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

type Consumer struct {
	rabbitURL      string
	mu             sync.RWMutex
	connection     *amqp.Connection
	channel        *amqp.Channel
	isConnecting   bool
	shutdownSignal chan struct{}
	consumerTag    string
}

func NewConsumer(url string, tag string) (*Consumer, error) {
	if tag == "" {
		tag = fmt.Sprintf("consumer-%d", time.Now().UnixNano())
	}

	c := &Consumer{
		rabbitURL:      url,
		shutdownSignal: make(chan struct{}),
		consumerTag:    tag,
	}

	err := c.tryConnect()
	if err != nil {
		log.Printf("[CONSUMER][WARN] Initial RabbitMQ connection failed: %v. Starting background reconnect attempts...", err)
		go c.ensureConnection(context.Background())
	}

	return c, nil
}

func (c *Consumer) establishSession() error {
	c.closeSession()

	log.Printf("[CONSUMER] Attempting to connect to %s", c.rabbitURL)
	conn, err := amqp.Dial(c.rabbitURL)
	if err != nil {
		return fmt.Errorf("failed to dial RabbitMQ: %w", err)
	}
	c.connection = conn
	log.Println("[CONSUMER] Connection established.")

	go c.watchConnection(conn)

	ch, err := c.connection.Channel()
	if err != nil {
		c.closeSession()
		return fmt.Errorf("failed to open a channel: %w", err)
	}
	c.channel = ch
	log.Println("[CONSUMER] Channel opened.")

	go c.watchChannel(ch)

	if err := c.declareConsumingSchema(c.channel); err != nil {
		c.closeSession()
		return fmt.Errorf("failed to declare RabbitMQ consuming schema: %w", err)
	}
	log.Println("[CONSUMER] Consuming schema declared successfully.")

	// Set QoS for consumer
	if err := c.channel.Qos(prefetchCount, 0, false); err != nil {
		c.closeSession()
		return fmt.Errorf("failed to set QoS: %w", err)
	}
	log.Printf("[CONSUMER] QoS set to prefetch count %d.", prefetchCount)

	return nil
}

func (c *Consumer) tryConnect() error {
	c.mu.Lock()
	defer c.mu.Unlock()
	if c.connection != nil && !c.connection.IsClosed() && c.channel != nil {
		log.Println("[CONSUMER] tryConnect() called, but connection is already active.")
		return nil
	}
	return c.establishSession()
}

func (c *Consumer) watchConnection(conn *amqp.Connection) {
	closeChan := conn.NotifyClose(make(chan *amqp.Error))
	select {
	case err := <-closeChan:
		c.mu.Lock()
		isCurrentConnection := (c.connection == conn)
		if isCurrentConnection {
			c.closeSession()
		}
		c.mu.Unlock()

		if err != nil && isCurrentConnection {
			log.Printf("[CONSUMER][WARN] RabbitMQ connection closed unexpectedly: %v. Initiating reconnect...", err)
			go c.ensureConnection(context.Background())
		} else if isCurrentConnection {
			log.Println("[CONSUMER] Connection closed gracefully.")
		}

	case <-c.shutdownSignal:
		log.Println("[CONSUMER] Stopping connection watch.")
		return
	}
}

func (c *Consumer) watchChannel(ch *amqp.Channel) {
	closeChan := ch.NotifyClose(make(chan *amqp.Error))
	select {
	case err := <-closeChan:
		c.mu.Lock()
		isCurrentChannel := (c.channel == ch)
		if isCurrentChannel {
			c.channel = nil
		}
		c.mu.Unlock()

		if err != nil && isCurrentChannel {
			log.Printf("[CONSUMER][WARN] RabbitMQ channel closed unexpectedly: %v. Initiating reconnect...", err)
			go c.ensureConnection(context.Background())
		} else if isCurrentChannel {
			log.Println("[CONSUMER] Channel closed gracefully.")
		}
	case <-c.shutdownSignal:
		log.Println("[CONSUMER] Stopping channel watch.")
		return
	}
}

func (c *Consumer) ensureConnection(ctx context.Context) {
	c.mu.Lock()
	if c.isConnecting {
		c.mu.Unlock()
		log.Println("[CONSUMER] Connection maintenance already in progress, skipping new cycle start.")
		return
	}
	c.isConnecting = true
	c.mu.Unlock()

	defer func() {
		c.mu.Lock()
		c.isConnecting = false
		c.mu.Unlock()
		log.Println("[CONSUMER] Exiting connection maintenance cycle.")
	}()

	log.Println("[CONSUMER] Starting connection maintenance cycle...")
	ticker := time.NewTicker(reconnectDelay)
	defer ticker.Stop()

	for {
		if c.CheckStatus() {
			log.Println("[CONSUMER] Connection is active. Stopping maintenance cycle.")
			return
		}

		log.Println("[CONSUMER] Attempting to reconnect...")
		err := c.tryConnect()
		if err == nil {
			log.Println("[CONSUMER] Reconnect successful during maintenance cycle.")
			return
		}

		log.Printf("[CONSUMER] Reconnect attempt failed in cycle: %v. Retrying after %v", err, reconnectDelay)

		select {
		case <-ctx.Done():
			log.Printf("[CONSUMER] Connection maintenance aborted by context: %v", ctx.Err())
			return
		case <-c.shutdownSignal:
			log.Println("[CONSUMER] Connection maintenance stopped by signal.")
			return
		case <-ticker.C:
		}
	}
}

func (c *Consumer) declareConsumingSchema(ch *amqp.Channel) error {
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

func (c *Consumer) StartConsuming() (<-chan amqp.Delivery, error) {
	c.mu.RLock()
	ch := c.channel
	connected := c.connection != nil && !c.connection.IsClosed() && ch != nil
	c.mu.RUnlock()

	if !connected {
		log.Printf("[CONSUMER][WARN] StartConsuming failed: %v", ErrRabbitMQNotConnected)
		go c.ensureConnection(context.Background())
		return nil, ErrRabbitMQNotConnected
	}

	msgs, err := ch.Consume(
		resultQueueName,
		c.consumerTag,
		false,
		false,
		false,
		false,
		nil,
	)
	if err != nil {
		log.Printf("[CONSUMER][ERROR] Failed to start consuming results: %v", err)
		if errors.Is(err, amqp.ErrClosed) {
			go c.ensureConnection(context.Background())
			return nil, ErrRabbitMQNotConnected
		}
		return nil, fmt.Errorf("rabbitmq consume failed: %w", err)
	}

	log.Printf("[CONSUMER] Started consuming results from queue '%s' with tag '%s'", resultQueueName, c.consumerTag)
	return msgs, nil
}

func (c *Consumer) CheckStatus() bool {
	c.mu.RLock()
	defer c.mu.RUnlock()
	return c.connection != nil && !c.connection.IsClosed() && c.channel != nil
}

func (c *Consumer) Shutdown() {
	log.Println("[CONSUMER] Closing consumer resources...")
	select {
	case <-c.shutdownSignal:
	default:
		close(c.shutdownSignal)
	}

	c.mu.Lock()
	defer c.mu.Unlock()
	c.closeSession()
	log.Println("[CONSUMER] Consumer resources closed.")
}

func (c *Consumer) closeSession() {
	if c.channel != nil {

		_ = c.channel.Cancel(c.consumerTag, false)
		_ = c.channel.Close()
		c.channel = nil
	}
	if c.connection != nil && !c.connection.IsClosed() {
		_ = c.connection.Close()
		c.connection = nil
	}
}
