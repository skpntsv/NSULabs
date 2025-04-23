package service

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"time"

	"manager/db"
	"manager/models"
	"manager/queue"

	"github.com/streadway/amqp"
	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
)

const (
	requestTimeout           = 10 * time.Minute
	numTaskParts             = 3
	pendingTaskRetryInterval = 10 * time.Second
	reconnectDelay           = 5 * time.Second
)

type Manager struct {
	Repository *db.Repository
	Publisher  *queue.Publisher
	cancel     context.CancelFunc
}

func NewManager(mongoClient *mongo.Client, rabbitURL string, dbName string) (*Manager, error) {
	newRepository := db.NewRepository(mongoClient, dbName)
	newPublisher, err := queue.NewPublisher(rabbitURL)
	if err != nil {
		return nil, err
	}
	ctx, cancel := context.WithCancel(context.Background())

	m := &Manager{
		Repository: newRepository,
		Publisher:  newPublisher,
		cancel:     cancel,
	}

	// Запускаем обработчик результатов параллельно
	go m.listenForResults(ctx)
	// Запускаем обработчик неотправленных задач
	go m.runPendingTaskPublisher(ctx)

	log.Println("Manager initialized successfully.")
	return m, nil
}

func (manager *Manager) StartCrackRequest(req models.CrackRequest, requestID string) (string, error) {
	// Запускаем горутину для отслеживания таймаута запроса
	go manager.setRequestTimeout(requestID)

	// Делим задачу на части и отправляем в RabbitMQ
	for i := 0; i < numTaskParts; i++ {
		workerReq := models.WorkerRequest{
			RequestID:  requestID,
			PartNumber: i,
			PartCount:  numTaskParts,
			Hash:       req.Hash,
			MaxLength:  req.MaxLength,
		}

		body, err := json.Marshal(workerReq)
		if err != nil {
			log.Printf("Failed to marshal task part %d for request %s: %v", i, requestID, err)
			continue
		}

		err = manager.Publisher.PublishTask(body)
		// Если отправка не удалась, сохраняем в MongoDB для повторной попытки
		if err != nil {
			log.Printf("Failed to publish task part %d for request %s to RabbitMQ: %v", i, requestID, err)
			manager.Repository.SavePendingTask(workerReq, body)
			continue
		}

		log.Printf("Sent task part %d for request %s", i, requestID)
	}

	filter := bson.M{"_id": requestID}
	manager.Repository.UpdateTaskStatusTo(requestID, models.StatusInProgress, filter)

	return requestID, nil
}

func (manager *Manager) runPendingTaskPublisher(ctx context.Context) {
	log.Println("Starting pending task publisher...")
	ticker := time.NewTicker(pendingTaskRetryInterval)
	defer ticker.Stop()

	for {
		select {
		case <-ctx.Done():
			log.Println("Stopping pending task publisher...")
			return
		case <-ticker.C:
			manager.tryRepublishPendingTasks()
		}
	}
}

func (manager *Manager) tryRepublishPendingTasks() {
	findCtx, findCancel, cursor := manager.Repository.AllRequestsCursor()
	defer findCancel()
	defer cursor.Close(findCtx)

	for cursor.Next(findCtx) {
		var task models.PendingTask

		err := manager.Publisher.PublishTask(task.TaskBody)
		if err == nil {
			log.Printf("Successfully republished pending task for request %s", task.RequestID)
			manager.Repository.DeletePendingTask(task.ID)
		} else {
			log.Printf("Failed to republish pending task %s. I will retry later.", task.RequestID)
		}
	}
}

func (manager *Manager) setRequestTimeout(requestID string) {
	time.Sleep(requestTimeout)

	filter := bson.M{"_id": requestID, "status": models.StatusInProgress}
	result := manager.Repository.UpdateTaskStatusTo(requestID, models.StatusError, filter)
	if result.ModifiedCount > 0 {
		log.Printf("Request %s Timeout", requestID)
	} else {
		// Статус уже был изменен
		log.Printf("Request %s already completed or timed out, timeout check ignored", requestID)
	}
}

func (m *Manager) listenForResults(ctx context.Context) {
	log.Println("Starting workers listener")

	for {
		// Проверяем главный контекст перед каждой попыткой подключения
		select {
		case <-ctx.Done():
			log.Println("Main context cancelled before attempting connection cycle.")
			return
		default:
			// Продолжаем попытку
		}

		if !m.Publisher.IsConnected() {
			log.Println("Attempting reconnect before consuming results")
			err := m.Publisher.Reconnect(ctx)
			if err != nil {
				log.Printf("Reconnect failed: %v. Stopping listener.", err)
			}
			log.Println("Reconnect successful.")
		}

		log.Println("Attempting to establish result consumer")
		var msgs <-chan amqp.Delivery
		var err error

		msgs, err = m.Publisher.ConsumeResults()
		if err != nil {
			log.Printf("Failed to establish result consumer: %v. Will retry connection after delay", err)

			select {
			case <-time.After(reconnectDelay):
				continue // Начать следующую итерацию внешнего цикла for
			case <-ctx.Done():
				log.Println("Main context cancelled while waiting to retry consumer setup.")
			}
		}

		log.Println("Result consumer established successfully. Waiting for results...")
	consumeLoop: // Метка для выхода из этого цикла
		for {
			select {
			case <-ctx.Done():
				log.Println("ain context cancelled while consuming results. Stopping listener.")
				return
			case d, ok := <-msgs:
				if !ok {
					log.Println("Result message channel closed. Breaking consume loop to attempt reconnect.")
					break consumeLoop
				}

				log.Printf("Manager received a result message: %s", d.Body)
				var res models.WorkerResponse
				json.Unmarshal(d.Body, &res)

				err = m.processWorkerResult(res)
				if err != nil {
					log.Printf("Manager failed to process worker result for request %s: %v", res.RequestID, err)
					d.Nack(false, true)
				} else {
					d.Ack(false)
				}
			}
		}

		// Если мы здесь, значит break consumeLoop был вызван (канал msgs закрылся)

		select {
		case <-time.After(reconnectDelay):
		case <-ctx.Done():
			log.Println("Main context cancelled immediately after channel closure.")
			return
		}
	}
}

func (m *Manager) processWorkerResult(res models.WorkerResponse) error {
	currentInfo, err := m.Repository.FindRequest(res.RequestID)
	if err != nil {
		if errors.Is(err, mongo.ErrNoDocuments) {
			log.Printf("Request %s not found in DB when processing result of part %d", res.RequestID, res.PartNumber)
			return nil
		}
		log.Printf("Failed to find request %s in DB: %v", res.RequestID, err)
		return fmt.Errorf("failed to query request %s: %w", res.RequestID, err)
	}
	if currentInfo.Status != models.StatusInProgress {
		log.Printf("Received result for already completed/failed request %s (status: %s, part: %d)", res.RequestID, currentInfo.Status, res.PartNumber)
		return nil
	}

	m.Repository.UpdateTaskAfterReceivePart(res, currentInfo)

	return nil
}

func (manager *Manager) GetRequestStatus(requestId string) (models.StatusResponse, bool) {
	requestInfo, err := manager.Repository.FindRequest(requestId)
	if err != nil {
		return models.StatusResponse{}, false
	}

	res := models.StatusResponse{
		Status:      requestInfo.Status,
		ProgressPct: 0,
		Data:        nil,
	}

	if requestInfo.TotalParts > 0 {
		res.ProgressPct = (float64(requestInfo.ReceivedParts) / float64(requestInfo.TotalParts)) * 100.0
	}
	if requestInfo.Status == models.StatusReady {
		res.ProgressPct = 100
	}

	dataCopy := make([]string, len(requestInfo.Data))
	copy(dataCopy, requestInfo.Data)
	res.Data = &dataCopy

	return res, true
}
