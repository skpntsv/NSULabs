package api

import (
	"encoding/json"
	"log"
	"net/http"

	"github.com/google/uuid"
	
	"manager/models"
	"manager/service"
)

type API struct {
	Manager *service.Manager
}

func NewAPI(manager *service.Manager) *API {
	return &API{
		Manager: manager,
	}
}

func (a *API) HandleCrackRequest(w http.ResponseWriter, r *http.Request) {
	var req models.CrackRequest
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		http.Error(w, "Invalid request", http.StatusBadRequest)
		return
	}

	requestID := uuid.New().String()
	a.Manager.Mutex.Lock()
	a.Manager.Requests[requestID] = &models.Status{
		Status: models.StatusNew, 
		Total: len(a.Manager.WorkerURLs),
	}
	a.Manager.Mutex.Unlock()

	log.Printf("[REQUEST] New request: %s (hash: %s, maxLength: %d)\n", requestID, req.Hash, req.MaxLength)
	go a.Manager.DistributeWork(requestID, req)
	json.NewEncoder(w).Encode(models.CrackResponse{RequestID: requestID})
}

func (a *API) HandleWorkerResponse(w http.ResponseWriter, r *http.Request) {
	var res models.WorkerResponse
	if err := json.NewDecoder(r.Body).Decode(&res); err != nil {
		http.Error(w, "Invalid response", http.StatusBadRequest)
		return
	}

	a.Manager.ProcessWorkerResponse(res)
	w.WriteHeader(http.StatusOK)
}

func (a *API) GetStatus(w http.ResponseWriter, r *http.Request) {
	requestID := r.URL.Query().Get("requestId")
	status, exists := a.Manager.GetRequestStatus(requestID)
	
	if exists {
		json.NewEncoder(w).Encode(status)
	} else {
		http.Error(w, "Request not found", http.StatusNotFound)
	}
}

func (a *API) RegisterRoutes() {
	http.HandleFunc("/api/hash/crack", a.HandleCrackRequest)
	http.HandleFunc("/internal/api/manager/hash/crack/request", a.HandleWorkerResponse)
	http.HandleFunc("/api/hash/status", a.GetStatus)
}