package service

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"manager/config"
	"manager/db"
	"manager/models"
	"manager/queue"
	"math"
	"time"

	"go.mongodb.org/mongo-driver/mongo"
)

const (
	requestTimeout           = 1 * time.Minute
	pendingTaskRetryInterval = 10 * time.Second
	reconnectDelay           = 5 * time.Second
	managerQueueName         = "manager_queue"
	workerQueueName          = "worker_queue"
)

type Manager struct {
	HashRequestRepo *db.HashRequestRepository
	WorkerTaskRepo  *db.WorkerTaskRepository
	Producer        *queue.Producer
	Consumer        *queue.Consumer
	cancelFunc      context.CancelFunc
	isShuttingDown  bool
}

func NewManager(mongoClient *mongo.Client, RabbitURI string, dbName string) (*Manager, error) {
	hashRepo := db.NewHashRequestRepository(mongoClient, dbName)
	workerTaskRepo := db.NewWorkerTaskRepository(mongoClient, dbName)

	producer, err := queue.NewProducer(RabbitURI)
	if err != nil {
		log.Printf("[MANAGER][WARN] Failed to initialize RabbitMQ producer initially: %v", err)
	}
	consumer, err := queue.NewConsumer(RabbitURI, "")
	if err != nil {
		log.Printf("[MANAGER][WARN] Failed to initialize RabbitMQ consumer initially: %v", err)
	}

	ctx, cancel := context.WithCancel(context.Background())

	m := &Manager{
		HashRequestRepo: hashRepo,
		WorkerTaskRepo:  workerTaskRepo,
		Producer:        producer,
		Consumer:        consumer,
		cancelFunc:      cancel,
	}

	go m.listenForResults(ctx)

	go m.runPendingTaskProducer(ctx)

	log.Println("Manager initialized successfully.")
	return m, nil
}

func (m *Manager) Shutdown() {
	log.Println("Shutting down manager...")
	m.isShuttingDown = true
	m.cancelFunc()
	if m.Producer != nil {
		m.Producer.Shutdown()
	}
	if m.Consumer != nil {
		m.Consumer.Shutdown()
	}
	log.Println("Manager shutdown complete.")
}

func (m *Manager) CreateCrackRequest(req models.CrackRequest) (string, error) {

	numParts := config.PartNumber

	requestInfo, err := m.HashRequestRepo.SaveNewRequest(req, numParts)
	if err != nil {
		log.Printf("[ERROR] Failed to save new request in DB for hash %s: %v", req.Hash, err)
		return "", fmt.Errorf("database error saving request: %w", err)
	}
	log.Printf("[REQUEST] Created request %s in DB (hash: %s, maxLen: %d, parts: %d)",
		requestInfo.ID, requestInfo.Hash, requestInfo.MaxLength, requestInfo.TotalParts)

	go func(info models.RequestInfoDB, originalReq models.CrackRequest) {
		defer func() {
			if r := recover(); r != nil {
				log.Printf("[PANIC] Recovered in startProcessing: %v", r)
				_, _ = m.HashRequestRepo.UpdateRequestStatusIf(
					info.ID, models.StatusNew, models.StatusError, "Internal panic during processing",
				)
				_, _ = m.HashRequestRepo.UpdateRequestStatusIf(
					info.ID, models.StatusInProgress, models.StatusError, "Internal panic during processing",
				)
			}
		}()
		m.startProcessing(info, originalReq)
	}(requestInfo, req)
	return requestInfo.ID, nil
}

func (m *Manager) startProcessing(requestInfo models.RequestInfoDB, originalReq models.CrackRequest) {
	requestID := requestInfo.ID

	timeoutCtx, timeoutCancel := context.WithTimeout(context.Background(), requestTimeout)
	defer timeoutCancel()
	go m.monitorRequestTimeout(timeoutCtx, requestID)

	updated, err := m.HashRequestRepo.UpdateRequestStatusIf(requestID, models.StatusNew, models.StatusInProgress)
	if err != nil {
		log.Printf("[ERROR] Failed to set request %s to IN_PROGRESS: %v. Aborting processing.", requestID, err)
		_, _ = m.HashRequestRepo.UpdateRequestStatusIf(
			requestID, models.StatusInProgress, models.StatusError, "DB error starting processing",
		)
		return
	}
	if !updated {
		log.Printf("[WARN] Request %s was not in NEW state when trying to start processing. Aborting.", requestID)
		return
	}
	log.Printf("[PROGRESS] Request %s status set to IN_PROGRESS", requestID)

	for i := 0; i < requestInfo.TotalParts; i++ {
		select {
		case <-timeoutCtx.Done():
			log.Printf("[TIMEOUT] Request %s timed out before sending part %d. Aborting task distribution.", requestID, i)
			return
		default:
		}

		workerReq := models.WorkerRequest{
			RequestID:  requestID,
			PartNumber: i,
			PartCount:  requestInfo.TotalParts,
			Hash:       originalReq.Hash,
			MaxLength:  originalReq.MaxLength,
		}

		body, jsonErr := json.Marshal(workerReq)
		if jsonErr != nil {
			log.Printf("[ERROR] Failed to marshal task part %d for request %s: %v. Skipping part.", i, requestID, jsonErr)
			errMsg := fmt.Sprintf("Failed to marshal task part %d: %v", i, jsonErr)
			_, _ = m.HashRequestRepo.UpdateRequestStatusIf(requestID, models.StatusInProgress, models.StatusError, errMsg)
			log.Printf("[ERROR] Request %s marked as ERROR due to marshalling issue.", requestID)
			return
		}

		publishErr := m.Producer.SendMessage(body)
		if publishErr != nil {
			log.Printf("[WARN] Failed to publish task part %d for request %s to RabbitMQ: %v. Saving as pending task.",
				i, requestID, publishErr)
			saveErr := m.WorkerTaskRepo.SaveWorkerTask(workerReq, body)
			if saveErr != nil {
				log.Printf("[ERROR] CRITICAL: Failed to save pending task for request %s part %d: %v. Marking request as ERROR.",
					requestID, i, saveErr)
				errMsg := fmt.Sprintf("Failed to publish and save task part %d", i)
				_, _ = m.HashRequestRepo.UpdateRequestStatusIf(requestID, models.StatusInProgress, models.StatusError, errMsg)
				return
			}
		} else {
			log.Printf("[PUBLISH] Sent task part %d for request %s", i, requestID)
		}
	}
	log.Printf("[PROGRESS] All tasks for request %s distributed or saved as pending.", requestID)
}

func (m *Manager) monitorRequestTimeout(ctx context.Context, requestID string) {
	select {
	case <-ctx.Done():
		err := ctx.Err()
		if errors.Is(err, context.DeadlineExceeded) {
			log.Printf("[TIMEOUT] Request %s reached timeout (%v)", requestID, requestTimeout)
			updated, updateErr := m.HashRequestRepo.UpdateRequestStatusIf(
				requestID, models.StatusInProgress, models.StatusError, "Request timed out",
			)
			if updateErr != nil {
				log.Printf("[ERROR] Failed to set request %s status to ERROR on timeout: %v", requestID, updateErr)
			} else if updated {
				log.Printf("[PROGRESS] Request %s marked as ERROR due to timeout.", requestID)
			} else {
				log.Printf("[INFO] Request %s already completed or failed when timeout check occurred.", requestID)
			}
		} else if err != nil {
			log.Printf("[INFO] Timeout monitoring for request %s cancelled: %v", requestID, err)
		} else {
			log.Printf("[INFO] Timeout monitoring for request %s finished without specific error.", requestID)
		}
	default:
	}
}

func (m *Manager) runPendingTaskProducer(ctx context.Context) {
	log.Println("Starting pending task publisher...")
	ticker := time.NewTicker(pendingTaskRetryInterval)
	defer ticker.Stop()

	for {
		select {
		case <-ctx.Done():
			log.Println("Stopping pending task publisher...")
			return
		case <-ticker.C:
			if m.isShuttingDown {
				log.Println("Pending task publisher stopped due to shutdown signal.")
				return
			}
			m.trySendPendingTasks(ctx)
		}
	}
}

func (m *Manager) trySendPendingTasks(ctx context.Context) {
	tasks, err := m.WorkerTaskRepo.FindPendingTasks()
	if err != nil {
		if errors.Is(err, context.Canceled) || errors.Is(err, context.DeadlineExceeded) {
			log.Println("Context cancelled during FindPendingTasks.")
			return
		}
		log.Printf("[ERROR] Failed to fetch pending tasks: %v", err)
		return
	}

	if len(tasks) == 0 {
		return
	}

	log.Printf("[REPUBLISH] Found %d pending tasks to republish.", len(tasks))

	if !m.Producer.CheckStatus() {
		log.Printf("[REPUBLISH] RabbitMQ not connected. Skipping republish attempt.")
		return
	}

	for _, task := range tasks {
		select {
		case <-ctx.Done():
			log.Println("Context cancelled during pending task republish loop.")
			return
		default:
		}

		err := m.Producer.SendMessage(task.TaskBody)
		if err == nil {
			log.Printf("[REPUBLISH] Successfully republished pending task %s (request %s)", task.ID, task.RequestID)
			delErr := m.WorkerTaskRepo.DeleteTaskByID(task.ID)
			if delErr != nil {
				log.Printf("[ERROR] Failed to delete republished pending task %s from DB: %v", task.ID, delErr)
			}
		} else {
			log.Printf("[WARN] Failed to republish pending task %s (request %s): %v. Will retry later.",
				task.ID, task.RequestID, err)
			if errors.Is(err, queue.ErrRabbitMQNotConnected) { // Предполагаем, что Publisher возвращает такую ошибку
				log.Printf("[REPUBLISH] Stopping current republish cycle because RabbitMQ is disconnected.")
				break
			}
		}
	}
}

func (m *Manager) listenForResults(ctx context.Context) {
	log.Println("Starting results listener...")

	for {
		select {
		case <-ctx.Done():
			log.Println("Results listener stopping due to context cancellation.")
			return
		default:
		}

		msgs, err := m.Consumer.StartConsuming()
		if err != nil {
			log.Printf("[ERROR] Failed to start consuming results: %v. Retrying after delay...", err)
			select {
			case <-time.After(reconnectDelay):
				continue
			case <-ctx.Done():
				log.Println("Results listener stopping while waiting to retry connection.")
				return
			}
		}

		log.Println("Results listener consuming messages...")

	consumeLoop:
		for {
			select {
			case <-ctx.Done():
				log.Println("Results listener stopping while consuming messages.")
				return

			case d, ok := <-msgs:
				if !ok {
					log.Println("[WARN] Results channel closed. Attempting to re-establish consumer.")
					break consumeLoop
				}

				log.Printf("[RECEIVE] Received result message: %s", string(d.Body))
				var res models.WorkerResponse
				if err := json.Unmarshal(d.Body, &res); err != nil {
					log.Printf("[ERROR] Failed to unmarshal worker result: %v. Body: %s. Sending Nack (reject).", err, string(d.Body))
					_ = d.Nack(false, false)
					continue
				}

				err = m.processWorkerResult(res)
				if err != nil {
					log.Printf("[ERROR] Failed to process worker result for request %s part %d: %v. Sending Nack (requeue).",
						res.RequestID, res.PartNumber, err)
					_ = d.Nack(false, true)
				} else {
					log.Printf("[ACK] Successfully processed result for request %s part %d. Sending Ack.",
						res.RequestID, res.PartNumber)
					_ = d.Ack(false)
				}
			}
		}

		log.Printf("Waiting %.1f seconds before attempting to restart consumer...", reconnectDelay.Seconds())
		select {
		case <-time.After(reconnectDelay):
		case <-ctx.Done():
			log.Println("Results listener stopping while waiting to reconnect after channel closure.")
			return
		}
	}
}

func (m *Manager) processWorkerResult(res models.WorkerResponse) error {
	updatedInfo, isComplete, err := m.HashRequestRepo.UpdateResultPart(res.RequestID, res.Words)

	if err != nil {
		if errors.Is(err, mongo.ErrNoDocuments) {
			log.Printf(
				"[WARN] Received result for request %s part %d, but request was not found or not in IN_PROGRESS state. Ignoring result.",
				res.RequestID, res.PartNumber)
			return nil
		}
		log.Printf("[ERROR] Database error processing result for request %s part %d: %v", res.RequestID, res.PartNumber, err)
		return fmt.Errorf("db error updating result part: %w", err)
	}

	log.Printf("[PROGRESS] Request %s: processed part %d/%d. Found %d words. Current status: %s.",
		updatedInfo.ID, updatedInfo.ReceivedParts, updatedInfo.TotalParts, len(res.Words), updatedInfo.Status)

	if isComplete {
		log.Printf("[COMPLETE] Request %s completed successfully!", updatedInfo.ID)
	}

	return nil
}

func (m *Manager) GetRequestStatus(requestID string) (models.StatusResponse, bool) {
	requestInfo, err := m.HashRequestRepo.FindRequestByID(requestID)
	if err != nil {
		if errors.Is(err, mongo.ErrNoDocuments) {
			return models.StatusResponse{}, false
		}
		log.Printf("[ERROR] Failed to get status for request %s: %v", requestID, err)
		return models.StatusResponse{}, false
	}

	res := models.StatusResponse{
		Status: requestInfo.Status,
		Data:   nil,
	}

	if requestInfo.TotalParts > 0 && requestInfo.Status == models.StatusInProgress {
		res.ProgressPct = math.Round((float64(requestInfo.ReceivedParts) / float64(requestInfo.TotalParts)) * 100.0)
	}

	if requestInfo.Status == models.StatusReady {
		dataCopy := make([]string, len(requestInfo.Data))
		copy(dataCopy, requestInfo.Data)
		res.Data = &dataCopy
		res.ProgressPct = 100.0
	}
	if requestInfo.Status == models.StatusError {
		res.ProgressPct = math.Round((float64(requestInfo.ReceivedParts) / float64(requestInfo.TotalParts)) * 100.0)
	}

	return res, true
}
