package db

import (
	"context"
	"log"
	"time"

	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"go.mongodb.org/mongo-driver/mongo/readpref"
)

type MongoDB struct {
	client       *mongo.Client
	db           *mongo.Database
	requestsColl *mongo.Collection
	tasksColl    *mongo.Collection
	progressColl *mongo.Collection
}

func NewMongoDB(uri string) (*MongoDB, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	// Настройка опций подключения с поддержкой реплика-сета
	clientOptions := options.Client().
		ApplyURI(uri).
		SetReplicaSet("rs0").
		SetWriteConcern(mongo.WriteConcern{W: "majority", J: true}).
		SetReadPreference(readpref.Primary())

	client, err := mongo.Connect(ctx, clientOptions)
	if err != nil {
		return nil, err
	}

	// Проверка подключения
	if err := client.Ping(ctx, readpref.Primary()); err != nil {
		return nil, err
	}

	db := client.Database("crackhash")

	// Создаем индексы для оптимизации запросов
	_, err = db.Collection("requests").Indexes().CreateOne(ctx, mongo.IndexModel{
		Keys:    bson.D{{"requestId", 1}},
		Options: options.Index().SetUnique(true),
	})
	if err != nil {
		return nil, err
	}

	_, err = db.Collection("tasks").Indexes().CreateOne(ctx, mongo.IndexModel{
		Keys:    bson.D{{"requestId", 1}, {"partNumber", 1}},
		Options: options.Index().SetUnique(true),
	})
	if err != nil {
		return nil, err
	}

	mongoDB := &MongoDB{
		client:       client,
		db:           db,
		requestsColl: db.Collection("requests"),
		tasksColl:    db.Collection("tasks"),
		progressColl: db.Collection("progress"),
	}

	return mongoDB, nil
}

func (m *MongoDB) Close() {
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	if err := m.client.Disconnect(ctx); err != nil {
		log.Printf("Error disconnecting from MongoDB: %v", err)
	}
}
