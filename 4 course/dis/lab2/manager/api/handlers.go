package api

import (
	"encoding/json"
	"log"
	"manager/models"
	"manager/service"
	"net/http"
	"os"            // Добавляем для работы с файлами
	"path/filepath" // Для безопасного пути к файлу
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
	w.Header().Set("Content-Type", "application/json")

	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		log.Printf("[API-ERROR] Failed to decode crack request: %v", err)
		http.Error(w, `{"error": "Invalid request body"}`, http.StatusBadRequest)
		return
	}
	defer r.Body.Close()

	requestID, err := a.Manager.CreateCrackRequest(req)
	if err != nil {
		log.Printf("[API-ERROR] Failed to create crack request via service: %v", err)
		http.Error(w, `{"error": "Failed to initiate crack request"}`, http.StatusInternalServerError)
		return
	}

	log.Printf("[API-INFO] Accepted crack request %s (hash: %s, maxLength: %d)", requestID, req.Hash, req.MaxLength)

	response := models.CrackResponse{RequestID: requestID}
	if err := json.NewEncoder(w).Encode(response); err != nil {
		log.Printf("[API-ERROR] Failed to encode crack response for request %s: %v", requestID, err)
	}
}

func (a *API) GetStatus(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

	requestID := r.URL.Query().Get("requestId")
	if requestID == "" {
		w.Header().Set("Content-Type", "application/json")
		http.Error(w, `{"error": "requestId query parameter is required"}`, http.StatusBadRequest)
		return
	}

	statusResponse, exists := a.Manager.GetRequestStatus(requestID)

	w.Header().Set("Content-Type", "application/json")

	if !exists {
		log.Printf("[API-WARN] Status requested for unknown ID: %s", requestID)
		http.Error(w, `{"error": "Request not found"}`, http.StatusNotFound)
		return
	}

	if err := json.NewEncoder(w).Encode(statusResponse); err != nil {
		log.Printf("[API-ERROR] Failed to encode status response for request %s: %v", requestID, err)
	}
}

func (a *API) RegisterRoutes() {
	http.HandleFunc("/", a.ServeUI)
	http.HandleFunc("/api/hash/crack", a.HandleCrackRequest)
	http.HandleFunc("/api/hash/status", a.GetStatus)
	log.Println("[API-INFO] Registered HTTP routes: /, /api/hash/crack, /api/hash/status")
}

func (a *API) ServeUI(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/" {
		http.NotFound(w, r)
		return
	}

	filePath := filepath.Join("static", "index.html")

	if _, err := os.Stat(filePath); os.IsNotExist(err) {
		log.Printf("[API-WARN] Static file not found: %s", filePath)
		http.NotFound(w, r)
		return
	} else if err != nil {
		log.Printf("[API-WARN] Error checking file stat %s: %v", filePath, err)
		http.Error(w, "Internal server error", http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "text/html")
	http.ServeFile(w, r, filePath)
	log.Printf("[API-INFO] Served file: %s", filePath)
}
