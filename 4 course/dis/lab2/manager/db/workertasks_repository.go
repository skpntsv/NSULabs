package db

import (
	"context"
	"log"
	"manager/config"
	"manager/models"

	"github.com/google/uuid"
	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
)

type WorkerTaskRepository struct {
	collection *mongo.Collection
}

func NewWorkerTaskRepository(mongoClient *mongo.Client, dbName string) *WorkerTaskRepository {
	return &WorkerTaskRepository{
		collection: mongoClient.Database(dbName).Collection(config.WorkerTasksCollectionName),
	}
}

func (r *WorkerTaskRepository) SaveWorkerTask(req models.WorkerRequest, body []byte) error {
	ctx, cancel := context.WithTimeout(context.Background(), config.DbTimeout)
	defer cancel()

	task := models.PendingTask{
		ID:        uuid.NewString(),
		RequestID: req.RequestID,
		TaskBody:  body,
	}

	if _, err := r.collection.InsertOne(ctx, task); err != nil {
		log.Printf("[ERROR] Failed to insert worker task for request %s: %v", req.RequestID, err)
		return err
	}

	return nil
}

func (r *WorkerTaskRepository) FindPendingTasks() ([]models.PendingTask, error) {
	ctx, cancel := context.WithTimeout(context.Background(), config.DbTimeout)
	defer cancel()

	cursor, err := r.collection.Find(ctx, bson.M{})
	if err != nil {
		log.Printf("[ERROR] Failed to find pending tasks: %v", err)
		return nil, err
	}
	defer cursor.Close(ctx)

	var tasks []models.PendingTask
	if err := cursor.All(ctx, &tasks); err != nil {
		log.Printf("[ERROR] Failed to decode pending tasks: %v", err)
		return nil, err
	}

	return tasks, nil
}

func (r *WorkerTaskRepository) DeleteTaskByID(id string) error {
	ctx, cancel := context.WithTimeout(context.Background(), config.DbTimeout)
	defer cancel()

	_, err := r.collection.DeleteOne(ctx, bson.M{"_id": id})
	if err != nil {
		log.Printf("[ERROR] Failed to delete pending task %s: %v", id, err)
	}
	return err
}
