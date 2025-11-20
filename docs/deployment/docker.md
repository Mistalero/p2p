# Docker Deployment

This guide explains how to deploy the P2P node using Docker.

## Prerequisites

- Docker Engine 20.10 or higher
- Docker Compose 1.29 or higher

## Quick Start

1. Clone the repository:
   ```bash
   git clone https://git.sourcecraft.dev/p2p-decentralized-emulation/p2p.git
   cd p2p/implementations/javascript
   ```

2. Build the Docker image:
   ```bash
   docker build -t p2p-node .
   ```

3. Run the container:
   ```bash
   docker run -p 3000:3000 -p 3001:3001 p2p-node
   ```

## Docker Compose

For easier management, you can use Docker Compose:

1. Create a `docker-compose.yml` file:
   ```yaml
   version: '3.8'
   services:
     p2p-node:
       build: .
       ports:
         - "3000:3000"
         - "3001:3001"
       volumes:
         - ./data:/app/data
   ```

2. Run the service:
   ```bash
   docker-compose up -d
   ```

## Configuration

You can configure the P2P node through environment variables:

- `PORT` - HTTP API port (default: 3000)
- `P2P_PORT` - P2P communication port (default: 3001)
- `DATA_DIR` - Directory for storing event logs (default: ./data)

Example with custom configuration:
```bash
docker run -p 3000:3000 -p 3001:3001 -e PORT=4000 -e P2P_PORT=4001 p2p-node
```

## Data Persistence

To ensure data persistence across container restarts, mount a volume to the data directory:

```bash
docker run -p 3000:3000 -p 3001:3001 -v /host/data:/app/data p2p-node
```

## Networking

When running multiple nodes, ensure each node has unique ports:

```bash
# Node 1
docker run -p 3000:3000 -p 3001:3001 p2p-node

# Node 2
docker run -p 3002:3000 -p 3003:3001 p2p-node
```

## Troubleshooting

### Port Already in Use

If you see an error about ports being in use, change the port mappings:

```bash
docker run -p 4000:3000 -p 4001:3001 p2p-node
```

### Connection Issues

Ensure that the P2P port is accessible from other nodes in the network. You may need to configure firewall rules or port forwarding.