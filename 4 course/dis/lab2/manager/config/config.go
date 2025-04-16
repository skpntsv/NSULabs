package config

import "time"

const (
	WorkerResponseTimeout = 5 * time.Minute
	WorkerRequestTimeout  = 2 * time.Second
)
