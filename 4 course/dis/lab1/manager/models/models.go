package models

type RequestStatus string

const (
	StatusNew          RequestStatus = "NEW"
	StatusInProgress   RequestStatus = "IN_PROGRESS"
	StatusReady        RequestStatus = "READY"
	StatusError        RequestStatus = "ERROR"
	StatusPartialReady RequestStatus = "PARTIAL_READY"
)

type CrackRequest struct {
	Hash      string `json:"hash"`
	MaxLength int    `json:"maxLength"`
}

type CrackResponse struct {
	RequestID string `json:"requestId"`
}

type Status struct {
	Status   RequestStatus
	Data     []string
	Total    int
	Received int
}

type StatusResponse struct {
	Status    RequestStatus `json:"status"`
	Data      *[]string     `json:"data"`
	ProgressPct float64     `json:"progressPct,omitempty"`
}

type WorkerRequest struct {
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

// Новая структура для получения прогресса от воркера
type WorkerProgressResponse struct {
	RequestID   string  `json:"requestId"`
	PartNumber  int     `json:"partNumber"`
	ProgressPct float64 `json:"progressPct"`
}