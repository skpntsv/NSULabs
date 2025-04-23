package main

import (
	"context"
	"log"
	"os"
	"os/signal"
	"syscall"

	"worker/service"
)

func main() {
	rabbitURL := os.Getenv("RABBIT_URI")

	worker, err := service.NewWorker(rabbitURL)
	if err != nil {
		log.Fatalf("Failed to create worker: %v", err)
	}

	ctx, stop := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer stop()

	err = worker.Run(ctx)
	if err != nil {
		log.Printf("Failed to start consuming: %v", err)
	}
}
