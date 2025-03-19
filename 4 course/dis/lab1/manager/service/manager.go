package service

import (
	"bytes"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"sync"
	"time"

	"manager/models"
)

type Manager struct {
	Requests   map[string]*models.Status
	Mutex      sync.Mutex
	WorkerURLs []string
}

func NewManager(workerURLs []string) *Manager {
	return &Manager{
		Requests:   make(map[string]*models.Status),
		WorkerURLs: workerURLs,
	}
}

func (m *Manager) DistributeWork(requestID string, req models.CrackRequest) {
	m.Mutex.Lock()
	m.Requests[requestID].Status = models.StatusInProgress
	m.Mutex.Unlock()

	workerCount := len(m.WorkerURLs)
	if workerCount == 0 {
		log.Printf("[ERROR] No available workers for request %s\n", requestID)
		m.Mutex.Lock()
		m.Requests[requestID].Status = models.StatusError
		m.Mutex.Unlock()
		return
	}

	wg := sync.WaitGroup{}
	for i, workerURL := range m.WorkerURLs {
		wg.Add(1)
		workerReq := models.WorkerRequest{
			RequestID:  requestID,
			PartNumber: i,
			PartCount:  workerCount,
			Hash:       req.Hash,
			MaxLength:  req.MaxLength,
		}
		go func(url string, req models.WorkerRequest) {
			defer wg.Done()
			if !m.SendTaskToWorker(url, req) {
				log.Printf("[WARNING] Worker %s failed, task %d will be retried.\n", url, req.PartNumber)
			}
		}(workerURL, workerReq)
	}
	wg.Wait()

	time.AfterFunc(5*time.Minute, func() {
		log.Printf("[TIMEOUT] Request %s timed out\n", requestID)
		m.Mutex.Lock()
		var status = m.Requests[requestID]
		if status.Status == models.StatusInProgress && status.Received > 0 {
			m.Requests[requestID].Status = models.StatusPartialReady
		} else {
			m.Requests[requestID].Status = models.StatusError
		}
		m.Mutex.Unlock()
	})
}

func (m *Manager) SendTaskToWorker(workerURL string, req models.WorkerRequest) bool {
	data, _ := json.Marshal(req)
	_, err := http.Post(workerURL+"/internal/api/worker/hash/crack/task", "application/json", bytes.NewBuffer(data))
	if err != nil {
		log.Printf("[ERROR] Failed to send task to %s: %v\n", workerURL, err)
		return false
	}
	log.Printf("[TASK] Sent task %s to %s\n", req.RequestID, workerURL)
	return true
}

func (m *Manager) ProcessWorkerResponse(res models.WorkerResponse) {
	m.Mutex.Lock()
	defer m.Mutex.Unlock()

	if req, exists := m.Requests[res.RequestID]; exists {
		req.Data = append(req.Data, res.Words...)
		req.Received++
		log.Printf("[RESPONSE] Received %d/%d from workers for %s\n", req.Received, req.Total, res.RequestID)
		if req.Received == req.Total {
			req.Status = models.StatusReady
			log.Printf("[COMPLETE] Request %s completed\n", res.RequestID)
		} else {
			req.Status = models.StatusPartialReady
		}
	}
}

func (m *Manager) CollectProgressFromWorkers(requestID string) float64 {
	m.Mutex.Lock()
	status, exists := m.Requests[requestID]
	m.Mutex.Unlock()

	if !exists || status.Status == models.StatusReady || status.Status == models.StatusError {
		if status.Status == models.StatusReady {
			return 100.0
		}
		return 0.0
	}

	totalProgress := 0.0
	activeWorkers := 0

	var wg sync.WaitGroup
	var progressMutex sync.Mutex

	for i, workerURL := range m.WorkerURLs {
		wg.Add(1)
		go func(url string, partNumber int) {
			defer wg.Done()

			progressResp, err := http.Get(
				fmt.Sprintf("%s/internal/api/worker/hash/crack/progress?requestId=%s&partNumber=%d", 
					url, requestID, partNumber))
			if err != nil {
				log.Printf("[ERROR] Failed to get progress from worker %s: %v\n", url, err)
				return
			}
			defer progressResp.Body.Close()

			var workerProgress models.WorkerProgressResponse
			if err := json.NewDecoder(progressResp.Body).Decode(&workerProgress); err != nil {
				log.Printf("[ERROR] Failed to decode progress response from worker %s: %v\n", url, err)
				return
			}

			progressMutex.Lock()
			totalProgress += workerProgress.ProgressPct
			activeWorkers++
			progressMutex.Unlock()
		}(workerURL, i)
	}

	c := make(chan struct{})
	go func() {
		defer close(c)
		wg.Wait()
	}()

	select {
	case <-c:
	case <-time.After(2 * time.Second):
		log.Printf("[WARNING] Progress collection timed out for request %s\n", requestID)
	}

	if activeWorkers == 0 {
		return 0.0
	}

	return totalProgress / float64(activeWorkers)
}

func (m *Manager) GetRequestStatus(requestID string) (models.StatusResponse, bool) {
	m.Mutex.Lock()
	req, exists := m.Requests[requestID]
	m.Mutex.Unlock()

	if exists {
		var data *[]string = nil
		if len(req.Data) > 0 {
			data = &req.Data
		}

		progressPct := 0.0
		if req.Status == models.StatusInProgress || req.Status == models.StatusPartialReady {
			progressPct = m.CollectProgressFromWorkers(requestID)
		} else if req.Status == models.StatusReady {
			progressPct = 100.0
		}

		return models.StatusResponse{
			Status:      req.Status,
			Data:        data,
			ProgressPct: progressPct,
		}, true
	}
	return models.StatusResponse{}, false
}
