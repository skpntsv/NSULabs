package main

import (
	"log"
	"net/http"
	"os"
	"strings"

	"manager/api"
	"manager/service"
)

func main() {
	workerEnv := os.Getenv("WORKER_URLS")
	workerURLs := strings.Split(workerEnv, ",")

	manager := service.NewManager(workerURLs)
	apiHandler := api.NewAPI(manager)

	apiHandler.RegisterRoutes()

	log.Println("[START] Manager started on port 8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
