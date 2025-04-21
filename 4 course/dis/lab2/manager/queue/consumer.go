package queue

import (
	"context"
	"encoding/json"
	"log"

	"github.com/streadway/amqp"
)

// Обработчик результатов от воркеров
type ResultHandler func(ctx context.Context, result WorkerResult) error

func (r *RabbitMQ) ConsumeResults(ctx context.Context, handler ResultHandler) (<-chan struct{}, error) {
	if !r.isConnected {
		return nil, amqp.ErrClosed
	}

	// Начинаем потребление сообщений из очереди результатов
	messages, err := r.resultChannel.Consume(
		ResultQueueName, // очередь
		"",              // consumer
		false,           // auto-ack (отключено для ручного подтверждения)
		false,           // exclusive
		false,           // no-local
		false,           // no-wait
		nil,             // args
	)
	if err != nil {
		return nil, err
	}

	done := make(chan struct{})

	go func() {
		defer close(done)

		for {
			select {
			case <-ctx.Done():
				return
			case msg, ok := <-messages:
				if !ok {
					log.Println("Results channel closed")
					return
				}

				var result WorkerResult
				if err := json.Unmarshal(msg.Body, &result); err != nil {
					log.Printf("Error unmarshaling result: %v", err)
					msg.Nack(false, true) // Переотправить сообщение
					continue
				}

				log.Printf("Received result for request %s, part %d",
					result.RequestID, result.PartNumber)

				if err := handler(ctx, result); err != nil {
					log.Printf("Error handling result: %v", err)
					msg.Nack(false, true) // Переотправить сообщение
					continue
				}

				// Подтверждаем успешную обработку
				if err := msg.Ack(false); err != nil {
					log.Printf("Error acknowledging message: %v", err)
				}
			}
		}
	}()

	return done, nil
}
