package queue

import (
	"errors"
	"time"
)

const (
	rabbitMQDefaultURL = "amqp://guest:guest@rabbitmq:5672/"
	taskExchangeName   = "tasks_exchange"
	taskQueueName      = "crachhash_tasks"
	resultQueueName    = "crackhash_results"
	taskRoutingKey     = "task.crack.request"
	resultRoutingKey   = "task.crack.result"
	ReconnectDelay     = 5 * time.Second
	PublishRetryDelay  = 2 * time.Second
	MaxPublishRetries  = 5
	prefetchCount      = 1
)

var ErrRabbitMQNotConnected = errors.New("rabbitmq not connected")
