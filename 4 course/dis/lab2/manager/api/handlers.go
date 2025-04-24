package api

import (
	"encoding/json"
	"log"
	"manager/models"
	"manager/service"
	"net/http"
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
	log.Println("[API-INFO] Registered HTTP routes: /api/hash/crack, /api/hash/status")
}

func (a *API) ServeUI(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "text/html")
	html := `
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>CrackHash UI</title>
</head>
<body>
	<h1>Crack Hash</h1>
	<form id="crack-form">
    	<label>Hash: <input type="text" id="hash" required></label><br>
    	<label>Max Length: <input type="number" id="maxLength" min="1" value="5" required></label><br>
    	<button type="submit">Crack</button>
	</form>

	<h2>Status</h2>
	<pre id="status">Waiting...</pre>

	<script>
		const form = document.getElementById('crack-form');
		const statusBox = document.getElementById('status');
		let interval = null;

		form.onsubmit = async (e) => {
		e.preventDefault();
		const hash = document.getElementById('hash').value;
		const maxLength = parseInt(document.getElementById('maxLength').value);

		const res = await fetch('/api/hash/crack', {
			method: 'POST',
			headers: { 'Content-Type': 'application/json' },
			body: JSON.stringify({ hash, maxLength })
		});

		const data = await res.json();
		const requestId = data.requestId;
		statusBox.textContent = 'Request sent. ID: ' + requestId;

		if (interval) clearInterval(interval);
		interval = setInterval(async () => {
			const statusRes = await fetch('/api/hash/status?requestId=' + requestId);
			if (statusRes.ok) {
				const statusData = await statusRes.json();
				statusBox.textContent = JSON.stringify(statusData, null, 2);
				if (statusData.status === "READY") 
					clearInterval(interval);
				} else {
					statusBox.textContent = 'Error: could not get status.';
					clearInterval(interval);
				}
			}, 500);
		};
	</script>
</body>
</html>
`
	_, _ = w.Write([]byte(html))
}
