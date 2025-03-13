# Этап сборки
FROM golang:1.23.0 AS builder

WORKDIR /app
COPY . ./worker/

# Переходим в каталог воркера
WORKDIR /app/worker

# Загружаем зависимости
RUN go mod download

# Собираем бинарный файл с указанием правильной архитектуры
RUN go build -o worker .

# Финальный образ
FROM debian:latest
WORKDIR /app

# Копируем бинарник из этапа сборки
COPY --from=builder /app/worker/worker .

EXPOSE 8081
CMD ["./worker"]
