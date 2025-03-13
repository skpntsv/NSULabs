package api

import (
	"encoding/json"
	"log"
	"net/http"

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

func (a *API) RegisterRoutes() {
	http.HandleFunc("/internal/api/worker/hash/crack/task", a.HandleTask)
}