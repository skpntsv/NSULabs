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

type Producer struct {
	rabbitURL      string
	mu             sync.RWMutex
	connection     *amqp.Connection
	channel        *amqp.Channel
	isConnecting   bool
	shutdownSignal chan struct{}
}

func NewProducer(url string) (*Producer, error) {
	if url == "" {
		url = rabbitMQDefaultURL
	}

	p := &Producer{
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

func (p *Producer) establishSession() error {
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

func (p *Producer) tryConnect() error {
	p.mu.Lock()
	defer p.mu.Unlock()
	if p.connection != nil && !p.connection.IsClosed() && p.channel != nil {
		log.Println("[PRODUCER] tryConnect() called, but connection is already active.")
		return nil
	}
	return p.establishSession()
}

func (p *Producer) watchConnection(conn *amqp.Connection) {
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

func (p *Producer) watchChannel(ch *amqp.Channel) {
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

func (p *Producer) ensureConnection(ctx context.Context) {
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
	ticker := time.NewTicker(reconnectDelay)
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

		log.Printf("[PRODUCER] Reconnect attempt failed in cycle: %v. Retrying after %v", err, reconnectDelay)

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

func (p *Producer) declarePublishingSchema(ch *amqp.Channel) error {
	err := ch.ExchangeDeclare(
		taskExchangeName, "direct", true, false, false, false, nil,
	)
	if err != nil {
		return fmt.Errorf("failed to declare exchange '%s': %w", taskExchangeName, err)
	}

	_, err = ch.QueueDeclare(
		taskQueueName, true, false, false, false, nil,
	)
	if err != nil {
		return fmt.Errorf("failed to declare queue '%s': %w", taskQueueName, err)
	}

	err = ch.QueueBind(
		taskQueueName, taskRoutingKey, taskExchangeName, false, nil,
	)
	if err != nil {
		return fmt.Errorf("failed to bind queue '%s' to exchange '%s': %w", taskQueueName, taskExchangeName, err)
	}
	return nil
}

func (p *Producer) SendMessage(body []byte) error {
	p.mu.RLock()
	ch := p.channel
	connected := p.connection != nil && !p.connection.IsClosed() && ch != nil
	p.mu.RUnlock()

	if !connected {
		log.Printf("[PRODUCER][WARN] SendMessage failed: %v", ErrRabbitMQNotConnected)
		go p.ensureConnection(context.Background())
		return ErrRabbitMQNotConnected
	}

	err := ch.Publish(
		taskExchangeName, taskRoutingKey,
		false,
		false,
		amqp.Publishing{
			ContentType:  "application/json",
			DeliveryMode: amqp.Persistent,
			Body:         body,
		})

	if err != nil {
		log.Printf("[PRODUCER][ERROR] Failed to publish task: %v", err)
		if errors.Is(err, amqp.ErrClosed) {
			go p.ensureConnection(context.Background())
			return ErrRabbitMQNotConnected
		}
		return fmt.Errorf("rabbitmq publish failed: %w", err)
	}

	return nil
}

func (p *Producer) CheckStatus() bool {
	p.mu.RLock()
	defer p.mu.RUnlock()
	return p.connection != nil && !p.connection.IsClosed() && p.channel != nil
}

func (p *Producer) Shutdown() {
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

func (p *Producer) closeSession() {
	if p.channel != nil {
		_ = p.channel.Close()
		p.channel = nil
	}
	if p.connection != nil && !p.connection.IsClosed() {
		_ = p.connection.Close()
		p.connection = nil
	}
}
