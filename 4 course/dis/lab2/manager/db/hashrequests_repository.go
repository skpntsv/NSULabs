package db

import (
	"context"
	"log"
	"manager/config"
	"manager/models"

	"github.com/google/uuid"
	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
)

type HashRequestRepository struct {
	collection *mongo.Collection
}

func NewHashRequestRepository(mongoClient *mongo.Client, dbName string) *HashRequestRepository {
	return &HashRequestRepository{
		collection: mongoClient.Database(dbName).Collection(config.HashRequestCollectionName),
	}
}

func (r *HashRequestRepository) SaveNewRequest(req models.CrackRequest, numTaskParts int) (models.RequestInfoDB, error) {
	ctx, cancel := context.WithTimeout(context.Background(), config.DbTimeout)
	defer cancel()

	requestID := uuid.NewString()
	requestInfo := models.RequestInfoDB{
		ID:            requestID,
		Hash:          req.Hash,
		MaxLength:     req.MaxLength,
		Status:        models.StatusNew,
		TotalParts:    numTaskParts,
		ReceivedParts: 0,
		Data:          []string{},
	}

	_, err := r.collection.InsertOne(ctx, requestInfo)
	if err != nil {
		log.Printf("[ERROR] Failed to insert new request %s: %v", requestID, err)
		return models.RequestInfoDB{ID: requestID}, err
	}

	log.Printf("[DB] Saved new request %s", requestID)
	return requestInfo, nil
}

func (r *HashRequestRepository) FindRequestByID(requestID string) (models.RequestInfoDB, error) {
	ctx, cancel := context.WithTimeout(context.Background(), config.DbTimeout)
	defer cancel()

	var requestInfo models.RequestInfoDB
	filter := bson.M{"_id": requestID}
	err := r.collection.FindOne(ctx, filter).Decode(&requestInfo)
	if err != nil {
		log.Printf("[ERROR] Failed to find request %s: %v", requestID, err)
	}
	return requestInfo, err
}

func (r *HashRequestRepository) UpdateRequestStatusIf(requestID string, oldStatus models.RequestStatus,
	newStatus models.RequestStatus, errorMsg ...string) (bool, error) {
	ctx, cancel := context.WithTimeout(context.Background(), config.DbTimeout)
	defer cancel()

	filter := bson.M{"_id": requestID, "status": oldStatus}
	updateDoc := bson.M{"status": newStatus}
	if len(errorMsg) > 0 && errorMsg[0] != "" {
		updateDoc["error_msg"] = errorMsg[0]
	}

	update := bson.M{"$set": updateDoc}

	result, err := r.collection.UpdateOne(ctx, filter, update)
	if err != nil {
		log.Printf("[ERROR] Failed to update status for request %s from %s to %s: %v", requestID, oldStatus, newStatus, err)
		return false, err
	}

	updated := result.ModifiedCount > 0
	if updated {
		log.Printf("[DB] Updated status for request %s from %s to %s", requestID, oldStatus, newStatus)
	}
	return updated, nil
}

func (r *HashRequestRepository) UpdateResultPart(requestID string, words []string) (updatedInfo models.RequestInfoDB,
	isComplete bool, err error) {
	ctx, cancel := context.WithTimeout(context.Background(), config.DbTimeout)
	defer cancel()

	filter := bson.M{
		"_id":    requestID,
		"status": models.StatusInProgress,
	}

	update := bson.M{
		"$inc": bson.M{"received_parts": 1},
		"$push": bson.M{
			"data": bson.M{
				"$each": words,
			},
		},
	}

	opts := options.FindOneAndUpdate().SetReturnDocument(options.After)

	err = r.collection.FindOneAndUpdate(ctx, filter, update, opts).Decode(&updatedInfo)
	if err != nil {
		if err == mongo.ErrNoDocuments {
			log.Printf("[WARN] Attempted to update result for request %s, but it was not found or not IN_PROGRESS", requestID)
			return models.RequestInfoDB{}, false, err
		}
		log.Printf("[ERROR] Failed to find and update result for request %s: %v", requestID, err)
		return models.RequestInfoDB{}, false, err
	}

	log.Printf("[DB] Updated request %s: received part %d/%d", updatedInfo.ID, updatedInfo.ReceivedParts, updatedInfo.TotalParts)

	if updatedInfo.ReceivedParts >= updatedInfo.TotalParts {
		isComplete = true
		statusUpdated, statusErr := r.UpdateRequestStatusIf(requestID, models.StatusInProgress, models.StatusReady)
		if statusErr != nil {
			log.Printf("[ERROR] Failed to set request %s status to READY after receiving all parts: %v", requestID, statusErr)
			err = statusErr
		} else if statusUpdated {
			log.Printf("[DB] Request %s marked as READY", requestID)
			updatedInfo.Status = models.StatusReady
		} else {
			log.Printf("[WARN] Request %s received all parts, but status was not IN_PROGRESS when trying to set READY.", requestID)
			freshInfo, findErr := r.FindRequestByID(requestID)
			if findErr == nil {
				updatedInfo = freshInfo
			}
		}
	}

	return updatedInfo, isComplete, err
}
