package service

import (
	"bytes"
	"crypto/md5"
	"encoding/hex"
	"encoding/json"
	"log"
	"math"
	"net/http"
	"strings"
	"sync"

	"worker/models"
)

type Worker struct {
	ManagerURL          string
	Charset             string
	ProgressByRequestID map[string]float64
	ProgressMutex       sync.RWMutex
}

func NewWorker(managerURL string) *Worker {
	return &Worker{
		ManagerURL:          managerURL,
		Charset:             "abcdefghijklmnopqrstuvwxyz0123456789",
		ProgressByRequestID: make(map[string]float64),
		ProgressMutex:       sync.RWMutex{},
	}
}

func (w *Worker) ProcessTask(task models.TaskRequest) {
	matches := w.BruteForceHash(task)

	if len(matches) == 0 {
		log.Printf("[COMPLETE] No matches found for task %s (part %d)", task.RequestID, task.PartNumber)

		w.SendResponse(task.RequestID, task.PartNumber, matches)
		return
	}

	log.Printf("[MATCH] Found %d matches for task %s (part %d): %v", 
					len(matches), task.RequestID, task.PartNumber, matches)
	w.SendResponse(task.RequestID, task.PartNumber, matches)
}

func (w *Worker) SendResponse(requestID string, partNumber int, matches []string) {
	response := models.WorkerResponse{
		RequestID:  requestID,
		Words:      matches,
		PartNumber: partNumber,
	}

	data, err := json.Marshal(response)
	if err != nil {
		log.Printf("[ERROR] Failed to marshal response: %v", err)
		return
	}

	resp, err := http.Post(
		w.ManagerURL+"/internal/api/manager/hash/crack/request",
		"application/json",
		bytes.NewBuffer(data),
	)

	if err != nil {
		log.Printf("[ERROR] Failed to send response to manager: %v", err)
		return
	}
	defer resp.Body.Close()

	log.Printf("[RESPONSE] Sent response to manager for task %s (part %d), status: %s",
		requestID, partNumber, resp.Status)
		
	w.ProgressMutex.Lock()
	delete(w.ProgressByRequestID, requestID)
	w.ProgressMutex.Unlock()
}

func (w *Worker) GetProgress(requestID string) float64 {
	w.ProgressMutex.RLock()
	defer w.ProgressMutex.RUnlock()
	
	if progress, exists := w.ProgressByRequestID[requestID]; exists {
		return progress
	}
	return 0.0
}

func (w *Worker) BruteForceHash(task models.TaskRequest) []string {
	targetHash := strings.ToLower(task.Hash)
	log.Printf("[PROCESSING] Searching for hash %s (max length: %d)", targetHash, task.MaxLength)

	matches := []string{}

	var totalCombinations int64 = 0
	for length := 1; length <= task.MaxLength; length++ {
		totalCombinations += int64(math.Pow(float64(len(w.Charset)), float64(length)))
	}

	partSize := totalCombinations / int64(task.PartCount)
	startIndex := int64(task.PartNumber) * partSize
	endIndex := startIndex + partSize
	if task.PartNumber == task.PartCount-1 {
		endIndex = totalCombinations
	}

	log.Printf("[PARTITION] Processing combinations %d to %d of %d total",
		startIndex, endIndex, totalCombinations)

	var combinationIndex int64 = 0
	var processedCount int64 = 0
	var workPartSize = endIndex - startIndex
	
	w.ProgressMutex.Lock()
	w.ProgressByRequestID[task.RequestID] = 0.0
	w.ProgressMutex.Unlock()

	for length := 1; length <= task.MaxLength; length++ {
		combinationsForLength := int64(math.Pow(float64(len(w.Charset)), float64(length)))

		if combinationIndex+combinationsForLength <= startIndex {
			combinationIndex += combinationsForLength
			continue
		}

		if combinationIndex >= endIndex {
			break
		}

		word := make([]byte, length)
		w.ProcessCombinations(word, 0, targetHash, &matches,
			combinationIndex, startIndex, endIndex, &combinationIndex, 
			&processedCount, workPartSize, task.RequestID)
	}

	w.ProgressMutex.Lock()
	w.ProgressByRequestID[task.RequestID] = 100.0
	w.ProgressMutex.Unlock()

	return matches
}

func (w *Worker) ProcessCombinations(word []byte, position int, targetHash string,
	matches *[]string, currentIndex, startIndex, endIndex int64, combinationIndex *int64, 
	processedCount *int64, workPartSize int64, requestID string) {

	if position == len(word) {
		if *combinationIndex >= startIndex && *combinationIndex < endIndex {
			hash := md5.Sum(word)
			if hex.EncodeToString(hash[:]) == targetHash {
				*matches = append(*matches, string(word))
			}
			
			(*processedCount)++
			
			if *processedCount%1000 == 0 {
				progressPct := float64(*processedCount) / float64(workPartSize) * 100.0
				w.ProgressMutex.Lock()
				w.ProgressByRequestID[requestID] = progressPct
				w.ProgressMutex.Unlock()
			}
		}

		(*combinationIndex)++
		return
	}

	if *combinationIndex+int64(math.Pow(float64(len(w.Charset)), float64(len(word)-position))) <= startIndex {
		*combinationIndex += int64(math.Pow(float64(len(w.Charset)), float64(len(word)-position)))
		return
	}

	if *combinationIndex >= endIndex {
		*combinationIndex += int64(math.Pow(float64(len(w.Charset)), float64(len(word)-position)))
		return
	}

	for i := 0; i < len(w.Charset); i++ {
		word[position] = w.Charset[i]
		w.ProcessCombinations(word, position+1, targetHash, matches,
			currentIndex, startIndex, endIndex, combinationIndex, processedCount, workPartSize, requestID)
	}
}