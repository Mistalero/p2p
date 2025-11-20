#!/bin/bash

# Script for automatic deployment of P2P node to Yandex Cloud
# Requires Yandex Cloud CLI and Docker to be installed

set -e  # Stop on any error

echo "Starting deployment of P2P node to Yandex Cloud..."

# Checking for required tools
echo "Checking for required tools..."
if ! command -v yc &> /dev/null; then
    echo "Yandex Cloud CLI not found. Please install it with:"
    echo "curl https://storage.yandexcloud.net/yandexcloud-yc/install.sh | bash"
    exit 1
fi

if ! command -v docker &> /dev/null; then
    echo "Docker not found. Please install Docker."
    exit 1
fi

# Parameters (can be changed)
REGISTRY_NAME="p2p-registry"
VM_NAME="p2p-node"
ZONE="ru-central1-a"
FOLDER_ID=$(yc config get folder-id 2>/dev/null)

if [ -z "$FOLDER_ID" ]; then
    echo "Failed to get folder-id from yc configuration. Please run yc init"
    exit 1
fi

echo "Using folder-id: $FOLDER_ID"

# Navigate to the application directory
cd "$(dirname "$0")/../implementations/javascript" || exit 1

echo "Building Docker image..."
docker build -t p2p-node .

echo "Configuring Docker to work with Yandex Container Registry..."
yc container registry configure-docker

echo "Creating Container Registry (if it doesn't already exist)..."
if ! yc container registry get --name $REGISTRY_NAME &>/dev/null; then
    yc container registry create --name $REGISTRY_NAME --folder-id $FOLDER_ID
fi

REGISTRY_ID=$(yc container registry get --name $REGISTRY_NAME --format json | jq -r .id)
echo "Using registry ID: $REGISTRY_ID"

echo "Uploading image to Container Registry..."
docker tag p2p-node cr.yandex/$REGISTRY_ID/p2p-node:latest
docker push cr.yandex/$REGISTRY_ID/p2p-node:latest

echo "Creating virtual machine with Container Optimized Image..."
yc compute instance create-container \
  --name $VM_NAME \
  --zone $ZONE \
  --public-ip \
  --container-image cr.yandex/$REGISTRY_ID/p2p-node:latest \
  --container-port 4001 \
  --container-stdin \
  --folder-id $FOLDER_ID

echo "Checking virtual machine status..."
yc compute instance get --name $VM_NAME --folder-id $FOLDER_ID

echo "Deployment completed successfully!"
echo "Your P2P node is now running in Yandex Cloud."
echo "To connect to the node, use the command:"
echo "yc compute instance get --name $VM_NAME --folder-id $FOLDER_ID"