# Этап сборки
FROM golang:1.23.0 AS builder
WORKDIR /app

COPY . ./manager/

# Переходим в каталог менеджера
WORKDIR /app/manager

# Загружаем зависимости
RUN go mod download

# Собираем бинарник с указанием правильной архитектуры
RUN go build -o manager .

# Финальный образ
FROM debian:latest
WORKDIR /app

# Копируем собранный бинарник из этапа сборки
COPY --from=builder /app/manager/manager .

EXPOSE 8080
CMD ["./manager"]
