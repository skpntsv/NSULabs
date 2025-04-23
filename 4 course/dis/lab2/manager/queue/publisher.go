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
	taskExchangeName   = "tasks_exchange"
	taskQueueName      = "tasks_queue"          // Очередь для отправки задач воркерам
	resultQueueName    = "results_queue"        // Очередь для получения результатов от воркеров
	taskRoutingKey     = "task.crack.request"   // Ключ маршрутизации для задач
	resultRoutingKey   = "task.crack.result"    // Ключ маршрутизации для результатов
	deadLetterExchange = "dead_letter_exchange" // Exchange для "мертвых" сообщений (опционально)
	deadLetterQueue    = "dead_letter_queue"    // Очередь для "мертвых" сообщений (опционально)
	reconnectDelay     = 5 * time.Second        // Пауза между попытками переподключения
	reconnectTimeout   = 5 * time.Minute        // Общий таймаут на переподключение
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

	ch.ExchangeDeclare(
		taskExchangeName, // name
		"direct",         // type
		true,             // durable
		false,            // auto-deleted
		false,            // internal
		false,            // no-wait
		nil,              // arguments
	)
	ch.QueueDeclare(
		taskQueueName, // name
		true,          // durable
		false,         // delete when unused
		false,         // exclusive
		false,         // no-wait
		nil,           // arguments
	)
	ch.QueueBind(
		taskQueueName,
		taskRoutingKey,
		taskExchangeName,
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
		resultQueueName,
		resultRoutingKey,
		taskExchangeName,
		false,
		nil,
	)

	return &Publisher{
		rabbitConn: conn,
		rabbitChan: ch,
		rabbitURL:  rabbitURL,
	}, nil
}

func (p *Publisher) PublishTask(body []byte) error {
	return p.rabbitChan.Publish(
		taskExchangeName,
		taskRoutingKey,
		false,
		false,
		amqp.Publishing{
			ContentType:  "application/json",
			DeliveryMode: amqp.Persistent,
			Body:         body,
		})
}

func (p *Publisher) ConsumeResults() (<-chan amqp.Delivery, error) {
	if p.rabbitChan == nil {
		return nil, fmt.Errorf("cannot consume results, RabbitMQ channel is not open or nil")
	}

	msgs, _ := p.rabbitChan.Consume(
		resultQueueName,    // queue
		"manager-consumer", // consumer tag
		false,              // auto-ack
		false,              // exclusive
		false,              // no-local
		false,              // no-wait
		nil,                // args
	)
	return msgs, nil
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
