package queue

type WorkerTask struct {
	RequestID  string `json:"requestId"`
	PartNumber int    `json:"partNumber"`
	PartCount  int    `json:"partCount"`
	Hash       string `json:"hash"`
	MaxLength  int    `json:"maxLength"`
}

type WorkerResult struct {
	RequestID  string   `json:"requestId"`
	Words      []string `json:"words"`
	PartNumber int      `json:"partNumber"`
}

type WorkerProgress struct {
	RequestID   string  `json:"requestId"`
	PartNumber  int     `json:"partNumber"`
	ProgressPct float64 `json:"progressPct"`
}
