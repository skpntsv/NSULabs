package api

import (
	"encoding/json"
	"log"
	"net/http"

	"manager/config"
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
	if r.Method != http.MethodPost {
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

	var req models.CrackRequest
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		log.Printf("[ERROR] Failed to decode request: %v\n", err)
		http.Error(w, "Invalid request", http.StatusBadRequest)
		return
	}

	requestInfo := a.Manager.Repository.SaveNewRequest(req, config.PartNumber)
	log.Printf("[REQUEST] New request: %s (hash: %s, maxLength: %d)\n", requestInfo.ID, req.Hash, req.MaxLength)

	go func() {
        defer func() {
            if r := recover(); r != nil {
                log.P("[PANIC] StartCrackRequest: %v", r)
            }
        }()
        a.Manager.StartCrackRequest(req, requestInfo.ID)
    }()

	json.NewEncoder(w).Encode(models.CrackResponse{RequestID: requestInfo.ID})
}

func (a *API) GetStatus(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

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
	http.HandleFunc("/api/hash/status", a.GetStatus)
}
