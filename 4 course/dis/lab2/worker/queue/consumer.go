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

type TaskConsumer struct {
	rabbitURL      string
	mu             sync.RWMutex
	connection     *amqp.Connection
	channel        *amqp.Channel
	isConnecting   bool
	shutdownSignal chan struct{}
	consumerTag    string
}

func NewTaskConsumer(url string, tag string) (*TaskConsumer, error) {
	if tag == "" {
		tag = fmt.Sprintf("worker-%d", time.Now().UnixNano())
	}

	c := &TaskConsumer{
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

func (c *TaskConsumer) establishSession() error {
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

	if err := c.channel.Qos(prefetchCount, 0, false); err != nil {
		c.closeSession()
		return fmt.Errorf("failed to set QoS: %w", err)
	}
	log.Printf("[CONSUMER] QoS set to prefetch count %d.", prefetchCount)

	return nil
}

func (c *TaskConsumer) tryConnect() error {
	c.mu.Lock()
	defer c.mu.Unlock()
	if c.connection != nil && !c.connection.IsClosed() && c.channel != nil {
		log.Println("[CONSUMER] tryConnect() called, but connection is already active.")
		return nil
	}
	return c.establishSession()
}

func (c *TaskConsumer) watchConnection(conn *amqp.Connection) {
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

func (c *TaskConsumer) watchChannel(ch *amqp.Channel) {
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

func (c *TaskConsumer) ensureConnection(ctx context.Context) {
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
	ticker := time.NewTicker(ReconnectDelay)
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

		log.Printf("[CONSUMER] Reconnect attempt failed in cycle: %v. Retrying after %v", err, ReconnectDelay)

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

func (c *TaskConsumer) declareConsumingSchema(ch *amqp.Channel) error {
	err := ch.ExchangeDeclare(
		taskExchangeName,
		"direct",
		true,
		false,
		false,
		false,
		nil,
	)
	if err != nil {
		return fmt.Errorf("failed to declare exchange '%s': %w", taskExchangeName, err)
	}

	_, err = ch.QueueDeclare(
		taskQueueName,
		true,
		false,
		false,
		false,
		nil,
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

func (c *TaskConsumer) ReceiveTasks() (<-chan amqp.Delivery, error) {
	c.mu.RLock()
	ch := c.channel
	connected := c.connection != nil && !c.connection.IsClosed() && ch != nil
	c.mu.RUnlock()

	if !connected {
		log.Printf("[WORKER-CONSUMER][WARN] ReceiveTasks failed: %v", ErrRabbitMQNotConnected)
		go c.ensureConnection(context.Background())
		return nil, ErrRabbitMQNotConnected
	}

	msgs, err := ch.Consume(
		taskQueueName,
		c.consumerTag,
		false,
		false,
		false,
		false,
		nil,
	)
	if err != nil {
		log.Printf("[CONSUMER][ERROR] Failed to start consuming tasks: %v", err)
		if errors.Is(err, amqp.ErrClosed) {
			go c.ensureConnection(context.Background())
			return nil, ErrRabbitMQNotConnected
		}
		return nil, fmt.Errorf("rabbitmq consume failed: %w", err)
	}

	log.Printf("[CONSUMER] Started consuming tasks from queue '%s' with tag '%s'", taskQueueName, c.consumerTag)
	return msgs, nil
}

func (c *TaskConsumer) CheckStatus() bool {
	c.mu.RLock()
	defer c.mu.RUnlock()
	return c.connection != nil && !c.connection.IsClosed() && c.channel != nil
}

func (c *TaskConsumer) Shutdown() {
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

func (c *TaskConsumer) closeSession() {
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
