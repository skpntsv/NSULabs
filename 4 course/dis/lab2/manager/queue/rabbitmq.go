package queue

import (
	"log"

	"github.com/streadway/amqp"
)

const (
	TaskQueueName      = "worker_tasks"
	ResultQueueName    = "worker_results"
	TaskExchangeName   = "worker_tasks_exchange"
	ResultExchangeName = "worker_results_exchange"
)

type RabbitMQ struct {
	conn          *amqp.Connection
	taskChannel   *amqp.Channel
	resultChannel *amqp.Channel
	isConnected   bool
}

func NewRabbitMQ(uri string) (*RabbitMQ, error) {
	rmq := &RabbitMQ{}

	if err := rmq.connect(uri); err != nil {
		return nil, err
	}

	return rmq, nil
}

func (r *RabbitMQ) connect(uri string) error {
	var err error

	// Подключаемся к RabbitMQ
	r.conn, err = amqp.Dial(uri)
	if err != nil {
		return err
	}

	// Создаем канал для задач
	r.taskChannel, err = r.conn.Channel()
	if err != nil {
		r.conn.Close()
		return err
	}

	// Создаем канал для результатов
	r.resultChannel, err = r.conn.Channel()
	if err != nil {
		r.taskChannel.Close()
		r.conn.Close()
		return err
	}

	// Настраиваем exchange для задач
	if err := r.taskChannel.ExchangeDeclare(
		TaskExchangeName, // имя
		"direct",         // тип
		true,             // durable
		false,            // auto-deleted
		false,            // internal
		false,            // no-wait
		nil,              // аргументы
	); err != nil {
		return err
	}

	// Настраиваем очередь для задач
	if _, err := r.taskChannel.QueueDeclare(
		TaskQueueName, // имя
		true,          // durable
		false,         // auto-delete
		false,         // exclusive
		false,         // no-wait
		nil,           // аргументы
	); err != nil {
		return err
	}

	// Привязываем очередь к exchange
	if err := r.taskChannel.QueueBind(
		TaskQueueName,    // имя очереди
		TaskQueueName,    // ключ маршрутизации
		TaskExchangeName, // имя exchange
		false,            // no-wait
		nil,              // аргументы
	); err != nil {
		return err
	}

	// Настраиваем exchange для результатов
	if err := r.resultChannel.ExchangeDeclare(
		ResultExchangeName, // имя
		"direct",           // тип
		true,               // durable
		false,              // auto-deleted
		false,              // internal
		false,              // no-wait
		nil,                // аргументы
	); err != nil {
		return err
	}

	// Настраиваем очередь для результатов
	if _, err := r.resultChannel.QueueDeclare(
		ResultQueueName, // имя
		true,            // durable
		false,           // auto-delete
		false,           // exclusive
		false,           // no-wait
		nil,             // аргументы
	); err != nil {
		return err
	}

	// Привязываем очередь к exchange
	if err := r.resultChannel.QueueBind(
		ResultQueueName,    // имя очереди
		ResultQueueName,    // ключ маршрутизации
		ResultExchangeName, // имя exchange
		false,              // no-wait
		nil,                // аргументы
	); err != nil {
		return err
	}

	// Устанавливаем QoS для каналов
	if err := r.taskChannel.Qos(1, 0, false); err != nil {
		return err
	}

	if err := r.resultChannel.Qos(1, 0, false); err != nil {
		return err
	}

	r.isConnected = true

	// Настраиваем обработку закрытия соединения
	go r.handleConnectionClose()

	return nil
}

func (r *RabbitMQ) handleConnectionClose() {
	<-r.conn.NotifyClose(make(chan *amqp.Error))
	log.Println("RabbitMQ connection closed")
	r.isConnected = false
}

func (r *RabbitMQ) IsConnected() bool {
	return r.isConnected
}

func (r *RabbitMQ) Close() {
	if r.taskChannel != nil {
		r.taskChannel.Close()
	}

	if r.resultChannel != nil {
		r.resultChannel.Close()
	}

	if r.conn != nil {
		r.conn.Close()
	}

	r.isConnected = false
}
