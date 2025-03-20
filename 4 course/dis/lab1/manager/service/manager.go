package service

import (
	"bytes"
	"encoding/json"
	"log"
	"math"
	"net/http"
	"sync"
	"time"

	"manager/config"
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

	time.AfterFunc(config.WorkerResponseTimeout, func() {
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
			ProgressPct: math.Round(progressPct*100) / 100,
		}, true
	}
	return models.StatusResponse{}, false
}
