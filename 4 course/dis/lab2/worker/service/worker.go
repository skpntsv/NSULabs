package service

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"sync"
	"time"

	"worker/config"
	"worker/models"
	"worker/queue"

	"github.com/google/uuid"
	"github.com/streadway/amqp"
)

type Worker struct {
	Charset             string
	TaskConsumer        *queue.TaskConsumer
	ResultProducer      *queue.ResultProducer
	ProgressByRequestID map[string]float64
	ProgressMutex       sync.RWMutex
}

func NewWorker(rabbitURL string) (*Worker, error) {
	workerTag := fmt.Sprintf("worker-%s", uuid.NewString())

	consumer, err := queue.NewTaskConsumer(rabbitURL, workerTag)
	if err != nil {
		log.Printf("[WARN] Failed to initialize RabbitMQ consumer initially: %v", err)
	}

	producer, err := queue.NewResultProducer(rabbitURL)
	if err != nil {
		log.Printf("[WARN] Failed to initialize RabbitMQ producer initially: %v", err)
	}

	w := &Worker{
		Charset:             config.Charset,
		TaskConsumer:        consumer,
		ResultProducer:      producer,
		ProgressByRequestID: make(map[string]float64),
		ProgressMutex:       sync.RWMutex{},
	}

	log.Println("Worker initialized and connected to RabbitMQ.")

	return w, nil
}

func (w *Worker) Run(ctx context.Context) error {
	log.Println("  Starting worker run loop...")

	var msgs <-chan amqp.Delivery
	var err error

	msgs, err = w.TaskConsumer.ReceiveTasks()
	if err != nil {
		log.Printf(" [ERROR] Failed to start consuming tasks initially: %v. Retrying...", err)
	}

	log.Println("  Run loop started.")
	for {
		if msgs == nil {
			log.Println("  Message channel is nil, attempting to restart consumer...")
			time.Sleep(queue.ReconnectDelay)
			newMsgs, err := w.TaskConsumer.ReceiveTasks()
			if err != nil {
				log.Printf(" [ERROR] Failed to restart consumer: %v. Will retry...", err)
				continue
			}
			msgs = newMsgs
			log.Println("Successfully restarted consumer.")
		}

		select {
		case <-ctx.Done():
			log.Println("Context cancelled. Shutting down...")
			w.Shutdown()
			return ctx.Err()

		case msg, ok := <-msgs:
			if !ok {
				log.Println("[WARN] Task channel closed unexpectedly. Will attempt to restart consumer.")
				msgs = nil
				continue
			}
			w.handleTaskDelivery(ctx, msg)
		}
	}
}

func (w *Worker) handleTaskDelivery(ctx context.Context, msg amqp.Delivery) {
	if len(msg.Body) == 0 {
		_ = msg.Nack(false, false)
		return
	}
	var task models.TaskRequest
	if err := json.Unmarshal(msg.Body, &task); err != nil || task.PartCount <= 0 {
		log.Printf("[ERROR] Invalid task message received. Discarding (Nack, no requeue). Body: %s", string(msg.Body))
		_ = msg.Nack(false, false)
		return
	}

	log.Printf("Processing task for RequestId: %s, Part: %d/%d", task.RequestID, task.PartNumber, task.PartCount)
	foundWords := w.BruteForceHash(task)
	response := models.WorkerResponse{RequestID: task.RequestID, Words: foundWords, PartNumber: task.PartNumber}
	body, marshalErr := json.Marshal(response)
	if marshalErr != nil {
		log.Printf("[ERROR] Failed to marshal result for %s part %d: %v. Discarding (Nack, no requeue).",
			task.RequestID, task.PartNumber, marshalErr)
		_ = msg.Nack(false, false)
		return
	}

	log.Printf("Attempting to publish result for RequestId: %s, Part: %d...", task.RequestID, task.PartNumber)
	retries := 0

	for retries < queue.MaxPublishRetries {
		select {
		case <-ctx.Done():
			log.Printf("Context cancelled before publishing result for %s part %d. Nacking (requeue).",
				task.RequestID, task.PartNumber)
			_ = msg.Nack(false, true)
			return
		default:
		}

		err := w.ResultProducer.SendResult(body)
		if err == nil {
			log.Printf("Successfully published result for %s part %d, words found: %d.",
				task.RequestID, task.PartNumber, len(foundWords))
			_ = msg.Ack(false)
			return
		}

		retries++
		log.Printf("[ERROR] Failed to publish result (attempt %d/%d) for %s part %d: %v.",
			retries, queue.MaxPublishRetries, task.RequestID, task.PartNumber, err)

		if !errors.Is(err, queue.ErrRabbitMQNotConnected) || retries >= queue.MaxPublishRetries {
			log.Printf("[ERROR] Unrecoverable error or max retries reached publishing result for %s part %d. Nacking (requeue).",
				task.RequestID, task.PartNumber)
			_ = msg.Nack(false, true)
			return
		}

		select {
		case <-time.After(queue.PublishRetryDelay):
		case <-ctx.Done():
			log.Printf("Context cancelled while waiting to retry publish for %s part %d. Nacking (requeue).",
				task.RequestID, task.PartNumber)
			_ = msg.Nack(false, true)
			return
		}
	}
	log.Printf("[ERROR] Exceeded max retries publishing result for %s part %d. Nacking (requeue).",
		task.RequestID, task.PartNumber)
	_ = msg.Nack(false, true)
}

func (w *Worker) Shutdown() {
	log.Println("Shutting down worker components...")
	if w.TaskConsumer != nil {
		w.TaskConsumer.Shutdown()
	}
	if w.ResultProducer != nil {
		w.ResultProducer.Shutdown()
	}
	log.Println("Worker components shut down.")
}

func (w *Worker) GetProgress(requestID string) float64 {
	w.ProgressMutex.RLock()
	defer w.ProgressMutex.RUnlock()

	if progress, exists := w.ProgressByRequestID[requestID]; exists {
		return progress
	}
	return 0.0
}
