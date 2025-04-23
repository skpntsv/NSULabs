package main

import (
	"context"
	"fmt"
	"log"
	"net/http"
	"os"
	"time"

	"manager/api"
	"manager/service"

	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
)

func main() {
	rabbitURL := os.Getenv("RABBIT_URI")
	mongoURLs := os.Getenv("MONGO_URLS")
	mongoDB := os.Getenv("MONGO_DB")

	client := initMongo(mongoURLs)
	defer func() {
		ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
		defer cancel()
		if err := client.Disconnect(ctx); err != nil {
			log.Fatal(err)
		}
	}()

	manager, err := service.NewManager(client, rabbitURL, mongoDB)
	if err != nil {
		log.Fatalf("Failed to create manager: %v", err)
	}

	apiHandler := api.NewAPI(manager)

	apiHandler.RegisterRoutes()

	log.Println("[START] Manager started on port 8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}

func initMongo(mongoURLs string) *mongo.Client {
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	client, err := mongo.Connect(ctx, options.Client().ApplyURI(mongoURLs))
	if err != nil {
		log.Fatal(err)
	}

	err = client.Ping(ctx, nil)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Connected to MongoDB successfully")
	return client
}
