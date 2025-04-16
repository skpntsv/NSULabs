package service

import (
	"bytes"
	"encoding/json"
	"log"
	"net/http"
	"sync"

	"worker/config"
	"worker/models"
)

type Worker struct {
	ManagerURL          string
	Charset             string
	ProgressByRequestID map[string]float64
	ProgressMutex       sync.RWMutex
}

func NewWorker(managerURL string) *Worker {
	return &Worker{
		ManagerURL:          managerURL,
		Charset:             config.Charset,
		ProgressByRequestID: make(map[string]float64),
		ProgressMutex:       sync.RWMutex{},
	}
}

func (w *Worker) ProcessTask(task models.TaskRequest) {
	matches := w.BruteForceHash(task)

	if len(matches) == 0 {
		log.Printf("[COMPLETE] No matches found for task %s (part %d)", task.RequestID, task.PartNumber)

		w.SendResponse(task.RequestID, task.PartNumber, matches)
		return
	}

	log.Printf("[MATCH] Found %d matches for task %s (part %d): %v",
		len(matches), task.RequestID, task.PartNumber, matches)
	w.SendResponse(task.RequestID, task.PartNumber, matches)
}

func (w *Worker) SendResponse(requestID string, partNumber int, matches []string) {
	response := models.WorkerResponse{
		RequestID:  requestID,
		Words:      matches,
		PartNumber: partNumber,
	}

	data, err := json.Marshal(response)
	if err != nil {
		log.Printf("[ERROR] Failed to marshal response: %v", err)
		return
	}

	resp, err := http.Post(
		w.ManagerURL+"/internal/api/manager/hash/crack/request",
		"application/json",
		bytes.NewBuffer(data),
	)

	if err != nil {
		log.Printf("[ERROR] Failed to send response to manager: %v", err)
		return
	}
	defer resp.Body.Close()

	log.Printf("[RESPONSE] Sent response to manager for task %s (part %d), status: %s",
		requestID, partNumber, resp.Status)

	w.ProgressMutex.Lock()
	delete(w.ProgressByRequestID, requestID)
	w.ProgressMutex.Unlock()
}

func (w *Worker) GetProgress(requestID string) float64 {
	w.ProgressMutex.RLock()
	defer w.ProgressMutex.RUnlock()

	if progress, exists := w.ProgressByRequestID[requestID]; exists {
		return progress
	}
	return 0.0
}
