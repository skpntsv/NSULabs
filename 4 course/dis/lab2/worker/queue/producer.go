package queue

import (
	"context"
	"errors"
	"fmt"
	"log"
	"sync"
	"time"

	"github.com/streadway/amqp"
)

type ResultProducer struct {
	rabbitURL      string
	mu             sync.RWMutex
	connection     *amqp.Connection
	channel        *amqp.Channel
	isConnecting   bool
	shutdownSignal chan struct{}
}

func NewResultProducer(url string) (*ResultProducer, error) {
	if url == "" {
		url = rabbitMQDefaultURL
	}

	p := &ResultProducer{
		rabbitURL:      url,
		shutdownSignal: make(chan struct{}),
	}

	err := p.tryConnect()
	if err != nil {
		log.Printf("[PRODUCER][WARN] Initial RabbitMQ connection failed: %v. Starting background reconnect attempts...", err)
		go p.ensureConnection(context.Background())
	}

	return p, nil
}

func (p *ResultProducer) establishSession() error {
	p.closeSession()

	log.Printf("[PRODUCER] Attempting to connect to %s", p.rabbitURL)
	conn, err := amqp.Dial(p.rabbitURL)
	if err != nil {
		return fmt.Errorf("failed to dial RabbitMQ: %w", err)
	}
	p.connection = conn
	log.Println("[PRODUCER] Connection established.")

	go p.watchConnection(conn)

	ch, err := p.connection.Channel()
	if err != nil {
		p.closeSession()
		return fmt.Errorf("failed to open a channel: %w", err)
	}
	p.channel = ch
	log.Println("[PRODUCER] Channel opened.")

	go p.watchChannel(ch)

	if err := p.declarePublishingSchema(p.channel); err != nil {
		p.closeSession()
		return fmt.Errorf("failed to declare RabbitMQ publishing schema: %w", err)
	}
	log.Println("[PRODUCER] Publishing schema declared successfully.")

	return nil
}

func (p *ResultProducer) tryConnect() error {
	p.mu.Lock()
	defer p.mu.Unlock()
	if p.connection != nil && !p.connection.IsClosed() && p.channel != nil {
		log.Println("[PRODUCER] tryConnect() called, but connection is already active.")
		return nil
	}
	return p.establishSession()
}

func (p *ResultProducer) watchConnection(conn *amqp.Connection) {
	closeChan := conn.NotifyClose(make(chan *amqp.Error))
	select {
	case err := <-closeChan:
		p.mu.Lock()
		isCurrentConnection := (p.connection == conn)
		if isCurrentConnection {
			p.closeSession()
		}
		p.mu.Unlock()

		if err != nil && isCurrentConnection {
			log.Printf("[PRODUCER][WARN] RabbitMQ connection closed unexpectedly: %v. Initiating reconnect...", err)
			go p.ensureConnection(context.Background())
		} else if isCurrentConnection {
			log.Println("[PRODUCER] Connection closed gracefully.")
		}

	case <-p.shutdownSignal:
		log.Println("[PRODUCER] Stopping connection watch.")
		return
	}
}

func (p *ResultProducer) watchChannel(ch *amqp.Channel) {
	closeChan := ch.NotifyClose(make(chan *amqp.Error))
	select {
	case err := <-closeChan:
		p.mu.Lock()
		isCurrentChannel := (p.channel == ch)
		if isCurrentChannel {
			p.channel = nil
		}
		p.mu.Unlock()

		if err != nil && isCurrentChannel {
			log.Printf("[PRODUCER][WARN] RabbitMQ channel closed unexpectedly: %v. Initiating reconnect...", err)
			go p.ensureConnection(context.Background())
		} else if isCurrentChannel {
			log.Println("[PRODUCER] Channel closed gracefully.")
		}
	case <-p.shutdownSignal:
		log.Println("[PRODUCER] Stopping channel watch.")
		return
	}
}

func (p *ResultProducer) ensureConnection(ctx context.Context) {
	p.mu.Lock()
	if p.isConnecting {
		p.mu.Unlock()
		log.Println("[PRODUCER] Connection maintenance already in progress, skipping new cycle start.")
		return
	}
	p.isConnecting = true
	p.mu.Unlock()

	defer func() {
		p.mu.Lock()
		p.isConnecting = false
		p.mu.Unlock()
		log.Println("[PRODUCER] Exiting connection maintenance cycle.")
	}()

	log.Println("[PRODUCER] Starting connection maintenance cycle...")
	ticker := time.NewTicker(ReconnectDelay)
	defer ticker.Stop()

	for {
		if p.CheckStatus() {
			log.Println("[PRODUCER] Connection is active. Stopping maintenance cycle.")
			return
		}

		log.Println("[PRODUCER] Attempting to reconnect...")
		err := p.tryConnect()
		if err == nil {
			log.Println("[PRODUCER] Reconnect successful during maintenance cycle.")
			return
		}

		log.Printf("[PRODUCER] Reconnect attempt failed in cycle: %v. Retrying after %v", err, ReconnectDelay)

		select {
		case <-ctx.Done():
			log.Printf("[PRODUCER] Connection maintenance aborted by context: %v", ctx.Err())
			return
		case <-p.shutdownSignal:
			log.Println("[PRODUCER] Connection maintenance stopped by signal.")
			return
		case <-ticker.C:
		}
	}
}

func (p *ResultProducer) declarePublishingSchema(ch *amqp.Channel) error {
	err := ch.ExchangeDeclare(
		taskExchangeName, "direct", true, false, false, false, nil,
	)
	if err != nil {
		return fmt.Errorf("failed to declare exchange '%s': %w", taskExchangeName, err)
	}

	_, err = ch.QueueDeclare(
		resultQueueName, true, false, false, false, nil,
	)
	if err != nil {
		return fmt.Errorf("failed to declare queue '%s': %w", resultQueueName, err)
	}

	err = ch.QueueBind(
		resultQueueName, resultRoutingKey, taskExchangeName, false, nil,
	)
	if err != nil {
		return fmt.Errorf("failed to bind queue '%s' to exchange '%s': %w", resultQueueName, taskExchangeName, err)
	}

	return nil
}

func (p *ResultProducer) SendResult(body []byte) error {
	p.mu.RLock()
	ch := p.channel
	connected := p.connection != nil && !p.connection.IsClosed() && ch != nil
	p.mu.RUnlock()

	if !connected {
		log.Printf("[PRODUCER][WARN] SendResult failed: %v", ErrRabbitMQNotConnected)
		go p.ensureConnection(context.Background())
		return ErrRabbitMQNotConnected
	}

	err := ch.Publish(
		taskExchangeName, resultRoutingKey, // Use result routing key
		false, false,
		amqp.Publishing{
			ContentType:  "application/json",
			DeliveryMode: amqp.Persistent,
			Body:         body,
		})

	if err != nil {
		log.Printf("[PRODUCER][ERROR] Failed to publish result: %v", err)
		if errors.Is(err, amqp.ErrClosed) {
			go p.ensureConnection(context.Background())
			return ErrRabbitMQNotConnected
		}
		return fmt.Errorf("rabbitmq publish failed: %w", err)
	}

	return nil
}

func (p *ResultProducer) CheckStatus() bool {
	p.mu.RLock()
	defer p.mu.RUnlock()
	return p.connection != nil && !p.connection.IsClosed() && p.channel != nil
}

func (p *ResultProducer) Shutdown() {
	log.Println("[PRODUCER] Closing producer resources...")
	select {
	case <-p.shutdownSignal:
	default:
		close(p.shutdownSignal)
	}

	p.mu.Lock()
	defer p.mu.Unlock()
	p.closeSession()
	log.Println("[PRODUCER] Producer resources closed.")
}

func (p *ResultProducer) closeSession() {
	if p.channel != nil {
		_ = p.channel.Close()
		p.channel = nil
	}
	if p.connection != nil && !p.connection.IsClosed() {
		_ = p.connection.Close()
		p.connection = nil
	}
}
