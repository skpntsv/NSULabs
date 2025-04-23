package queue

import (
	"context"
	"fmt"
	"log"
	"time"

	"github.com/streadway/amqp"
)

const (
	rabbitMQDefaultURL = "amqp://guest:guest@rabbitmq:5672/"
	taskExchangeName   = "tasks_exchange"     // Имя exchange (direct)
	taskQueueName      = "tasks_queue"        // Очередь задач для воркера
	resultQueueName    = "results_queue"      // Очередь результатов (для объявления, но не для потребления)
	taskRoutingKey     = "task.crack.request" // Ключ для получения задач
	resultRoutingKey   = "task.crack.result"  // Ключ для отправки результатов
	reconnectDelay     = 5 * time.Second
)

type Publisher struct {
	rabbitConn *amqp.Connection
	rabbitChan *amqp.Channel
	rabbitURL  string
}

func NewPublisher(rabbitURL string) (*Publisher, error) {
	if rabbitURL == "" {
		rabbitURL = rabbitMQDefaultURL
	}

	conn, err := amqp.Dial(rabbitURL)
	if err != nil {
		return nil, fmt.Errorf("failed to connect to RabbitMQ: %w", err)
	}
	ch, err := conn.Channel()
	if err != nil {
		conn.Close()
		return nil, fmt.Errorf("failed to open a channel: %w", err)
	}

	if err := ch.Qos(
		1, // prefetch count = 1
		0,
		false,
	); err != nil {
		ch.Close()
		conn.Close()
		return nil, fmt.Errorf("failed to set QoS: %w", err)
	}

	err = ch.ExchangeDeclare(
		taskExchangeName, // name
		"direct",         // type
		true,             // durable
		false,            // auto-deleted
		false,            // internal
		false,            // no-wait
		nil,              // arguments
	)
	if err != nil {
		ch.Close()
		conn.Close()
		return nil, fmt.Errorf("failed to declare exchange '%s': %w", taskExchangeName, err)
	}

	ch.QueueDeclare(
		taskQueueName, // name
		true,          // durable
		false,         // delete when unused
		false,         // exclusive
		false,         // no-wait
		nil,           // arguments
	)
	ch.QueueBind(
		taskQueueName,    // queue name
		taskRoutingKey,   // routing key
		taskExchangeName, // exchange
		false,
		nil,
	)

	ch.QueueDeclare(
		resultQueueName, // name
		true,            // durable
		false,           // delete when unused
		false,           // exclusive
		false,           // no-wait
		nil,             // arguments
	)
	ch.QueueBind(
		resultQueueName,  // queue name
		resultRoutingKey, // routing key for results
		taskExchangeName, // exchange
		false,
		nil)

	return &Publisher{
		rabbitConn: conn,
		rabbitChan: ch,
		rabbitURL:  rabbitURL,
	}, nil
}

func (p *Publisher) ConsumeResults() (<-chan amqp.Delivery, error) {
	if p.rabbitChan == nil {
		return nil, fmt.Errorf("cannot consume results, RabbitMQ channel is not open or nil")
	}

	msgs, _ := p.rabbitChan.Consume(
		taskQueueName,      // queue
		"manager-consumer", // consumer tag
		false,              // auto-ack
		false,              // exclusive
		false,              // no-local
		false,              // no-wait
		nil,                // args
	)
	return msgs, nil
}

func (p *Publisher) PublishResponse(body []byte) error {
	return p.rabbitChan.Publish(
		taskExchangeName, // exchange
		resultRoutingKey, // routing key for results
		false,
		false,
		amqp.Publishing{
			ContentType:  "application/json",
			DeliveryMode: amqp.Persistent,
			Body:         body,
		})
}

func (p *Publisher) IsConnected() bool {
	if p.rabbitConn == nil || p.rabbitConn.IsClosed() {
		return false
	}
	return true
}

func (p *Publisher) Reconnect(ctx context.Context) error {
	log.Println("Starting reconnect")
	ticker := time.NewTicker(reconnectDelay)
	defer ticker.Stop()

	for {
		if p.IsConnected() {
			log.Println("Connection is already active during reconnect attempt.")
			return nil
		}

		p.Close()
		err := p.connect()
		if err == nil {
			log.Println("Reconnect successful.")
			return nil
		}

		log.Printf("Reconnect attempt failed: %v. Retrying after %v", err, reconnectDelay)

		select {
		case <-ctx.Done():
			log.Printf("Reconnect context cancelled: %v", ctx.Err())
			return fmt.Errorf("reconnect aborted by context: %w", ctx.Err())
		case <-ticker.C:
			// Продолжаем следующую итерацию цикла for.
		}
	}
}

func (p *Publisher) connect() error {
	var err error
	conn, err := amqp.Dial(p.rabbitURL)
	if err != nil {
		return fmt.Errorf("failed to dial RabbitMQ: %w", err)
	}
	p.rabbitConn = conn

	ch, err := p.rabbitConn.Channel()
	if err != nil {
		p.rabbitConn.Close()
		p.rabbitConn = nil
		return fmt.Errorf("failed to open a channel: %w", err)
	}
	p.rabbitChan = ch

	if err := p.declareSchema(); err != nil {
		p.rabbitChan.Close()
		p.rabbitConn.Close()
		p.rabbitChan = nil
		p.rabbitConn = nil
		return fmt.Errorf("failed to declare RabbitMQ schema: %w", err)
	}

	if err := p.rabbitChan.Qos(1, 0, false); err != nil {
		p.rabbitChan.Close()
		p.rabbitConn.Close()
		p.rabbitChan = nil
		p.rabbitConn = nil
		return fmt.Errorf("failed to set QoS: %w", err)
	}

	return nil
}

func (p *Publisher) declareSchema() error {
	err := p.rabbitChan.ExchangeDeclare(
		taskExchangeName, // name
		"direct",         // type
		true,             // durable
		false,            // auto-deleted
		false,            // internal
		false,            // no-wait
		nil,              // arguments
	)
	if err != nil {
		return err
	}

	p.rabbitChan.QueueDeclare(
		taskQueueName, // name
		true,          // durable
		false,         // delete when unused
		false,         // exclusive
		false,         // no-wait
		nil,           // arguments
	)
	p.rabbitChan.QueueBind(
		taskQueueName,    // queue name
		taskRoutingKey,   // routing key
		taskExchangeName, // exchange
		false,
		nil,
	)

	p.rabbitChan.QueueDeclare(
		resultQueueName, // name
		true,            // durable
		false,           // delete when unused
		false,           // exclusive
		false,           // no-wait
		nil,             // arguments
	)
	p.rabbitChan.QueueBind(
		resultQueueName,  // queue name
		resultRoutingKey, // routing key for results
		taskExchangeName, // exchange
		false,
		nil)

	return nil
}

func (p *Publisher) Close() {
	if p.rabbitConn != nil && !p.rabbitConn.IsClosed() {
		if err := p.rabbitConn.Close(); err != nil {
			log.Printf("Warn: Error closing RabbitMQ connection: %v", err)
		}
		p.rabbitConn = nil
	}
}
