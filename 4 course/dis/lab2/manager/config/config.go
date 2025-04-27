package config

import "time"

const (
	PartNumber            = 100
	DbTimeout             = 10 * time.Second
	WorkerTasksCollectionName = "worker_tasks"
	HashRequestCollectionName = "hash_requests"
)
