package api

import (
	"encoding/json"
	"log"
	"net/http"
	"strconv"

	"worker/models"
	"worker/service"
)

type API struct {
	Worker *service.Worker
}

func NewAPI(worker *service.Worker) *API {
	return &API{
		Worker: worker,
	}
}

func (a *API) HandleTask(w http.ResponseWriter, r *http.Request) {
	var task models.TaskRequest
	if err := json.NewDecoder(r.Body).Decode(&task); err != nil {
		http.Error(w, "Invalid request", http.StatusBadRequest)
		log.Printf("[ERROR] Failed to decode request: %v", err)
		return
	}

	log.Printf("[TASK] Received task: %s (part %d of %d)", task.RequestID, task.PartNumber, task.PartCount)

	go a.Worker.ProcessTask(task)

	w.WriteHeader(http.StatusAccepted)
}

func (a *API) HandleGetProgress(w http.ResponseWriter, r *http.Request) {
	requestID := r.URL.Query().Get("requestId")
	partNumberStr := r.URL.Query().Get("partNumber")

	// Конвертация partNumber в int
	partNumber, err := strconv.Atoi(partNumberStr)
	if err != nil {
		http.Error(w, `{"error": "invalid partNumber"}`, http.StatusBadRequest)
		return
	}
	progress := a.Worker.GetProgress(requestID)

	response := models.ProgressResponse{
		RequestID:   requestID,
		PartNumber:  partNumber,
		ProgressPct: progress,
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(response)
}

func (a *API) RegisterRoutes() {
	http.HandleFunc("/internal/api/worker/hash/crack/task", a.HandleTask)
	http.HandleFunc("/internal/api/worker/hash/crack/progress", a.HandleGetProgress)
}
