package main

import (
	"log"
	"net/http"
	"os"

	"manager/api"
	"manager/db"
	"manager/service"
)

func main() {
	rabbitURL := os.Getenv("RABBIT_URI")
	mongoURI := os.Getenv("MONGO_URLS")
	mongoDB := os.Getenv("MONGO_DB")

	mongoClient := db.InitMongo(mongoURI)
	defer mongoClient.Disconnect(nil)

	manager, err := service.NewManager(mongoClient, rabbitURL, mongoDB)
	if err != nil {
		log.Fatalf("Failed to create manager: %v", err)
	}

	apiHandler := api.NewAPI(manager)
	apiHandler.RegisterRoutes()

	log.Println("Manager started on :8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
