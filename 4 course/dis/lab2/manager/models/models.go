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

type RequestInfoDB struct {
	ID            string        `bson:"_id"`
	Hash          string        `bson:"hash"`
	MaxLength     int           `bson:"max_length"`
	Status        RequestStatus `bson:"status"`
	TotalParts    int           `bson:"total_parts"`
	ReceivedParts int           `bson:"received_parts"`
	Data          []string      `bson:"data,omitempty"`
	ErrorMsg      string        `bson:"error_msg,omitempty"`
}

type StatusResponse struct {
	Status      RequestStatus `json:"status"`
	Data        *[]string     `json:"data"`
	ProgressPct float64       `json:"progressPct,omitempty"`
}

type PendingTask struct {
	ID        string `bson:"_id"`
	RequestID string `bson:"request_id"`
	TaskBody  []byte `bson:"task_body"`
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

type WorkerProgressResponse struct {
	RequestID   string  `json:"requestId"`
	PartNumber  int     `json:"partNumber"`
	ProgressPct float64 `json:"progressPct"`
}
