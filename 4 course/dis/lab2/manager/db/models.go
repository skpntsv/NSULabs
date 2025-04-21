package db

type RequestStatus string

type CrackRequestDB struct {
	RequestID  string        `bson:"requestId"`
	Hash       string        `bson:"hash"`
	MaxLength  int           `bson:"maxLength"`
	Status     RequestStatus `bson:"status"`
	CreatedAt  int64         `bson:"createdAt"`
	UpdatedAt  int64         `bson:"updatedAt"`
	Words      []string      `bson:"words,omitempty"`
	TasksTotal int           `bson:"tasksTotal"`
	TasksDone  int           `bson:"tasksDone"`
}

type WorkerTaskDB struct {
	RequestID  string `bson:"requestId"`
	PartNumber int    `bson:"partNumber"`
	Completed  bool   `bson:"completed"`
	Assigned   bool   `bson:"assigned"`
	MessageID  string `bson:"messageId,omitempty"`
	RetryCount int    `bson:"retryCount"`
}

type WorkerProgressDB struct {
	RequestID   string  `bson:"requestId"`
	PartNumber  int     `bson:"partNumber"`
	ProgressPct float64 `bson:"progressPct"`
	UpdatedAt   int64   `bson:"updatedAt"`
}
