package service

import (
	"encoding/json"
	"fmt"
	"log"
	"manager/config"
	"manager/models"
	"net/http"
	"sync"
	"time"
)

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
	case <-time.After(config.WorkerRequestTimeout):
		log.Printf("[WARNING] Progress collection timed out for request %s\n", requestID)
	}

	if activeWorkers == 0 {
		return 0.0
	}

	return totalProgress / float64(activeWorkers)
}
