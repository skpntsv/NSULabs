package main

import (
	"log"
	"net/http"
	"os"

	"worker/api"
	"worker/service"
)

func main() {
	managerURL := os.Getenv("MANAGER_URL")
	if managerURL == "" {
		managerURL = "http://manager:8080"
	}

	port := os.Getenv("PORT")
	if port == "" {
		port = "8081"
	}

	worker := service.NewWorker(managerURL)
	apiHandler := api.NewAPI(worker)
	
	apiHandler.RegisterRoutes()

	log.Printf("[START] Worker started on port %s", port)
	log.Printf("[CONFIG] Manager URL: %s", managerURL)
	log.Fatal(http.ListenAndServe(":"+port, nil))
}