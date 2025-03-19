package models

type TaskRequest struct {
	RequestID  string `json:"requestId"`
	PartNumber int    `json:"partNumber"`
	PartCount  int    `json:"partCount"`
	Hash       string `json:"hash"`
	MaxLength  int    `json:"maxLength"`
}

type WorkerResponse struct {
	RequestID  string   `json:"requestId"`
	Words      []string `json:"words"`
	PartNumber int      `json:"partNumber"`
}

type ProgressResponse struct {
	RequestID   string  `json:"requestId"`
	PartNumber  int     `json:"partNumber"`
	ProgressPct float64 `json:"progressPct"`
}