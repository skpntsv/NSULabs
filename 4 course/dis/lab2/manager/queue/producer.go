package queue

import (
	"context"
	"encoding/json"

	"github.com/google/uuid"
	"github.com/streadway/amqp"
)

func (r *RabbitMQ) PublishTask(ctx context.Context, task WorkerTask) (string, error) {
	if !r.isConnected {
		return "", amqp.ErrClosed
	}

	body, err := json.Marshal(task)
	if err != nil {
		return "", err
	}

	// Создаем уникальный ID сообщения
	messageID := generateMessageID()

	// Публикуем сообщение с persistent delivery mode
	err = r.taskChannel.Publish(
		TaskExchangeName, // exchange
		TaskQueueName,    // routing key
		false,            // mandatory
		false,            // immediate
		amqp.Publishing{
			ContentType:  "application/json",
			DeliveryMode: amqp.Persistent, // Гарантирует сохранение в случае перезапуска
			MessageId:    messageID,
			Body:         body,
		},
	)

	if err != nil {
		return "", err
	}

	return messageID, nil
}

// Вспомогательная функция для генерации MessageID
func generateMessageID() string {
	// Реализация генерации ID сообщения
	// Например, с использованием UUID
	return "msg-" + uuid.New().String()
}
