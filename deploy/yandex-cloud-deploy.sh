#!/bin/bash

# Скрипт автоматического деплоя P2P узла в Yandex Cloud
# Требует установленного Yandex Cloud CLI и Docker

set -e  # Остановка при любой ошибке

echo "Начинаем деплой P2P узла в Yandex Cloud..."

# Проверка наличия необходимых инструментов
echo "Проверяем наличие необходимых инструментов..."
if ! command -v yc &> /dev/null; then
    echo "Yandex Cloud CLI не найден. Пожалуйста, установите его с помощью:"
    echo "curl https://storage.yandexcloud.net/yandexcloud-yc/install.sh | bash"
    exit 1
fi

if ! command -v docker &> /dev/null; then
    echo "Docker не найден. Пожалуйста, установите Docker."
    exit 1
fi

# Параметры (можно изменить)
REGISTRY_NAME="p2p-registry"
VM_NAME="p2p-node"
ZONE="ru-central1-a"
FOLDER_ID=$(yc config get folder-id 2>/dev/null)

if [ -z "$FOLDER_ID" ]; then
    echo "Не удалось получить folder-id из конфигурации yc. Пожалуйста, выполните yc init"
    exit 1
fi

echo "Используем folder-id: $FOLDER_ID"

# Переход в директорию с приложением
cd "$(dirname "$0")/../implementations/javascript" || exit 1

echo "Собираем Docker-образ..."
docker build -t p2p-node .

echo "Настраиваем Docker для работы с Yandex Container Registry..."
yc container registry configure-docker

echo "Создаем Container Registry (если еще не существует)..."
if ! yc container registry get --name $REGISTRY_NAME &>/dev/null; then
    yc container registry create --name $REGISTRY_NAME --folder-id $FOLDER_ID
fi

REGISTRY_ID=$(yc container registry get --name $REGISTRY_NAME --format json | jq -r .id)
echo "Используем registry ID: $REGISTRY_ID"

echo "Загружаем образ в Container Registry..."
docker tag p2p-node cr.yandex/$REGISTRY_ID/p2p-node:latest
docker push cr.yandex/$REGISTRY_ID/p2p-node:latest

echo "Создаем виртуальную машину с Container Optimized Image..."
yc compute instance create-container \
  --name $VM_NAME \
  --zone $ZONE \
  --public-ip \
  --container-image cr.yandex/$REGISTRY_ID/p2p-node:latest \
  --container-port 4001 \
  --container-stdin \
  --folder-id $FOLDER_ID

echo "Проверяем статус виртуальной машины..."
yc compute instance get --name $VM_NAME --folder-id $FOLDER_ID

echo "Деплой завершен успешно!"
echo "Ваш P2P узел теперь запущен в Yandex Cloud."
echo "Для подключения к узлу используйте команду:"
echo "yc compute instance get --name $VM_NAME --folder-id $FOLDER_ID"