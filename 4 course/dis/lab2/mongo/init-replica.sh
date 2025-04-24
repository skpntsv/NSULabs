#!/bin/bash

# Ожидаем немного, чтобы все узлы точно запустились
echo "Waiting for MongoDB nodes to stabilize..."
sleep 15

# Пытаемся инициализировать реплика-сет
echo "Attempting to initiate replica set..."
mongosh --host mongo1:27017 --quiet --eval "
try {
    rs_status = rs.status();
    print('Replica set already initialized. Status OK:', rs_status.ok);
} catch (e) {
    if (e.message.includes('no replset config') || e.message.includes('not found replset config') || e.message.includes('run rs.initiate')) {
        print('Replica set not initialized, attempting initiation...');
        rs.initiate({ _id: 'rs0', members: [ { _id: 0, host: 'mongo1:27017', priority: 1 }, { _id: 1, host: 'mongo2:27017', priority: 1 }, { _id: 2, host: 'mongo3:27017', priority: 1 } ]});
        print('Replica set initiation command sent.');
        sleep(5000);
        print('Checking status after initiation...');
        rs_status = rs.status();
        print('Current replica set status OK:', rs_status.ok);
    } else {
        print('An unexpected error occurred during rs.status():', e);
    }
}
"

echo "Final replica set status check:"
mongosh --host mongo1:27017 --quiet --eval "printjson(rs.status())"

echo "Mongo replica set initialization script finished."