package config

import "time"

const (
	PartNumber            = 90
	DbTimeout             = 10 * time.Second
	WorkerTasksCollectionName = "worker_tasks"
	HashRequestCollectionName = "hash_requests"
)
