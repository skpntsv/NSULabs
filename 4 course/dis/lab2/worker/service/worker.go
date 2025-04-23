package service

import (
	"context"
	"encoding/json"
	"log"
	"sync"
	"time"

	"worker/config"
	"worker/models"
	"worker/queue"

	"github.com/streadway/amqp"
)

type Worker struct {
	ManagerURL          string
	Charset             string
	Publisher           queue.Publisher
	ProgressByRequestID map[string]float64
	ProgressMutex       sync.RWMutex
}

func NewWorker(rabbitURL string) (*Worker, error) {
	newPublisher, err := queue.NewPublisher(rabbitURL)
	if err != nil {
		return nil, err
	}

	w := &Worker{
		Charset:             config.Charset,
		Publisher:           *newPublisher,
		ProgressByRequestID: make(map[string]float64),
		ProgressMutex:       sync.RWMutex{},
	}

	log.Println("Worker initialized and connected to RabbitMQ.")

	return w, nil
}

func (w *Worker) StartConsuming(ctx context.Context) error {

	for {
		// Проверяем главный контекст перед каждой попыткой подключения/потребления
		select {
		case <-ctx.Done():
			log.Println("Main context cancelled before attempting connection cycle.")
			return ctx.Err()
		default:
			// Продолжаем попытку
		}

		if !w.Publisher.IsConnected() {
			log.Println("Not connected. Attempting reconnect before consuming")
			// содержит цикл с переподключением
			err := w.Publisher.Reconnect(ctx)
			if err != nil {
				log.Printf("Reconnect failed: %v. Worker stopping.", err)
				return err
			}
			log.Println("Reconnect successful.")
		}

		log.Println("Attempting to establish consumer...")
		msgs, err := w.Publisher.ConsumeResults()
		if err != nil {
			log.Printf("Failed to establish consumer: %v. Will retry connection after delay...", err)
			// Ждем перед следующей попыткой во внешнем цикле
			select {
			case <-time.After(config.PublishRetryDelay):
				continue // Начать следующую итерацию внешнего цикла for
			case <-ctx.Done():
				log.Println("Main context cancelled while waiting to retry consumer setup.")
				return ctx.Err()
			}
		}

		// внутренний цикл потребления сообщений
		log.Println("Consumer established successfully. Waiting for messages...")
	consumeLoop:
		for {
			select {
			case <-ctx.Done():
				log.Println("Main context cancelled while consuming. Worker stopping.")
				// Сообщение (если было взято) не Ack'ается, вернется в очередь
				return ctx.Err()
			case msg, ok := <-msgs:
				if !ok {
					log.Println("RabbitMQ message channel closed. Breaking consume loop to attempt reconnect.")
					break consumeLoop // Выходим из внутреннего цикла for, чтобы внешний цикл попробовал переподключиться
				}
				w.processNewTask(ctx, msg)
			}
		}

		// Если мы здесь, значит break consumeLoop был вызван (канал msgs закрылся)
		select {
		case <-time.After(1 * time.Second):
		case <-ctx.Done():
			log.Println("Main context cancelled immediately after channel closure.")
			return ctx.Err()
		}

	}
}

func (w *Worker) processNewTask(ctx context.Context, msg amqp.Delivery) {
	if len(msg.Body) == 0 {
		_ = msg.Nack(false, false)
		return
	}
	var task models.TaskRequest
	err := json.Unmarshal(msg.Body, &task)
	if err != nil {
		log.Printf("Failed to unmarshal message body: %v. Body: %s", err, string(msg.Body))
		_ = msg.Nack(false, false)
		return
	}
	if task.PartCount <= 0 {
		log.Printf("Received task with invalid PartCount <= 0. RequestId: %s, Part: %d/%d", task.RequestID, task.PartNumber, task.PartCount)
		_ = msg.Nack(false, false)
		return
	}

	log.Printf("Processing task for RequestId: %s, Part: %d/%d", task.RequestID, task.PartNumber, task.PartCount)
	foundWords := w.BruteForceHash(task)
	response := models.WorkerResponse{RequestID: task.RequestID, Words: foundWords, PartNumber: task.PartNumber}
	body, _ := json.Marshal(response)

	log.Printf("Attempting to publish result for RequestId: %s, Part: %d...", task.RequestID, task.PartNumber)
	for { // Бесконечный цикл попыток отправки
		select {
		case <-ctx.Done():
			log.Printf("Context cancelled during publish attempt for task %s part %d. Aborting publish and exiting without Ack.", task.RequestID, task.PartNumber)
			return
		default:
			// Продолжаем попытку отправки
		}

		err = w.Publisher.PublishResponse(body)
		if err == nil {
			log.Printf("Successfully published result for RequestId: %s, Part: %d, words: %v", task.RequestID, task.PartNumber, response.Words)
			msg.Ack(false)
			return
		}

		log.Printf("Failed to publish result for task %s part %d: %v. Will retry after reconnect attempt.", task.RequestID, task.PartNumber, err)

		// Пытаемся переподключиться.
		log.Println("Attempting reconnect...")
		reconnectErr := w.Publisher.Reconnect(ctx)
		if reconnectErr != nil {
			log.Printf("Reconnect failed for task %s part %d because context was cancelled: %v. Exiting without Ack.", task.RequestID, task.PartNumber, reconnectErr)
			return
		}
		log.Println("Reconnect successful. Retrying publish")
		// Цикл for продолжится и снова попытается вызвать PublishResponse
		time.Sleep(1 * time.Second)

	}
}

func (w *Worker) GetProgress(requestID string) float64 {
	w.ProgressMutex.RLock()
	defer w.ProgressMutex.RUnlock()

	if progress, exists := w.ProgressByRequestID[requestID]; exists {
		return progress
	}
	return 0.0
}
