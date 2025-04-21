package db

import (
	"context"
	"time"

	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"

	"manager/models"
)

// Создание нового запроса
func (m *MongoDB) CreateRequest(ctx context.Context, request CrackRequestDB) error {
	now := time.Now().Unix()
	request.CreatedAt = now
	request.UpdatedAt = now

	// Используем транзакцию для обеспечения атомарности
	session, err := m.client.StartSession()
	if err != nil {
		return err
	}
	defer session.EndSession(ctx)

	callback := func(sessionContext mongo.SessionContext) (interface{}, error) {
		// Вставляем запрос
		_, err := m.requestsColl.InsertOne(sessionContext, request)
		if err != nil {
			return nil, err
		}

		// Создаем задачи для каждой части
		var tasks []interface{}
		for i := 0; i < request.TasksTotal; i++ {
			task := WorkerTaskDB{
				RequestID:  request.RequestID,
				PartNumber: i,
				Completed:  false,
				Assigned:   false,
				RetryCount: 0,
			}
			tasks = append(tasks, task)
		}

		if len(tasks) > 0 {
			_, err = m.tasksColl.InsertMany(sessionContext, tasks)
			if err != nil {
				return nil, err
			}
		}

		return nil, nil
	}

	// Запуск транзакции с проверкой записи в большинство нод
	_, err = session.WithTransaction(ctx, callback, options.Transaction().
		SetWriteConcern(mongo.WriteConcern{W: "majority", J: true}))

	return err
}

// Получение запроса по ID
func (m *MongoDB) GetRequest(ctx context.Context, requestID string) (*CrackRequestDB, error) {
	var request CrackRequestDB

	err := m.requestsColl.FindOne(ctx, bson.M{"requestId": requestID}).Decode(&request)
	if err != nil {
		return nil, err
	}

	return &request, nil
}

// Обновление статуса запроса
func (m *MongoDB) UpdateRequestStatus(ctx context.Context, requestID string, status models.RequestStatus) error {
	update := bson.M{
		"$set": bson.M{
			"status":    status,
			"updatedAt": time.Now().Unix(),
		},
	}

	_, err := m.requestsColl.UpdateOne(
		ctx,
		bson.M{"requestId": requestID},
		update,
	)

	return err
}

// Добавление найденных слов
func (m *MongoDB) AddWords(ctx context.Context, requestID string, words []string) error {
	update := bson.M{
		"$addToSet": bson.M{"words": bson.M{"$each": words}},
		"$set":      bson.M{"updatedAt": time.Now().Unix()},
		"$inc":      bson.M{"tasksDone": 1},
	}

	_, err := m.requestsColl.UpdateOne(
		ctx,
		bson.M{"requestId": requestID},
		update,
	)

	return err
}

// Пометка задачи как назначенной
func (m *MongoDB) MarkTaskAssigned(ctx context.Context, requestID string, partNumber int, messageID string) error {
	update := bson.M{
		"$set": bson.M{
			"assigned":  true,
			"messageId": messageID,
		},
	}

	_, err := m.tasksColl.UpdateOne(
		ctx,
		bson.M{"requestId": requestID, "partNumber": partNumber},
		update,
	)

	return err
}

// Пометка задачи как выполненной
func (m *MongoDB) MarkTaskCompleted(ctx context.Context, requestID string, partNumber int) error {
	update := bson.M{
		"$set": bson.M{
			"completed": true,
		},
	}

	_, err := m.tasksColl.UpdateOne(
		ctx,
		bson.M{"requestId": requestID, "partNumber": partNumber},
		update,
	)

	return err
}

// Получение незавершенных задач
func (m *MongoDB) GetUncompletedTasks(ctx context.Context) ([]WorkerTaskDB, error) {
	cursor, err := m.tasksColl.Find(
		ctx,
		bson.M{"completed": false},
	)
	if err != nil {
		return nil, err
	}
	defer cursor.Close(ctx)

	var tasks []WorkerTaskDB
	if err := cursor.All(ctx, &tasks); err != nil {
		return nil, err
	}

	return tasks, nil
}

// Обновление прогресса выполнения
func (m *MongoDB) UpdateProgress(ctx context.Context, progress WorkerProgressDB) error {
	update := bson.M{
		"$set": bson.M{
			"progressPct": progress.ProgressPct,
			"updatedAt":   time.Now().Unix(),
		},
	}

	opts := options.Update().SetUpsert(true)

	_, err := m.progressColl.UpdateOne(
		ctx,
		bson.M{"requestId": progress.RequestID, "partNumber": progress.PartNumber},
		update,
		opts,
	)

	return err
}

// Получение прогресса для запроса
func (m *MongoDB) GetProgressForRequest(ctx context.Context, requestID string) ([]WorkerProgressDB, error) {
	cursor, err := m.progressColl.Find(
		ctx,
		bson.M{"requestId": requestID},
	)
	if err != nil {
		return nil, err
	}
	defer cursor.Close(ctx)

	var progresses []WorkerProgressDB
	if err := cursor.All(ctx, &progresses); err != nil {
		return nil, err
	}

	return progresses, nil
}

// Увеличение счетчика повторных попыток для задачи
func (m *MongoDB) IncrementTaskRetryCount(ctx context.Context, requestID string, partNumber int) error {
	update := bson.M{
		"$inc": bson.M{"retryCount": 1},
		"$set": bson.M{"assigned": false, "messageId": ""},
	}

	_, err := m.tasksColl.UpdateOne(
		ctx,
		bson.M{"requestId": requestID, "partNumber": partNumber},
		update,
	)

	return err
}
