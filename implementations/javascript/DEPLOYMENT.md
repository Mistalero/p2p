# Deployment of P2P Decentralized Emulation System

## Cloud Platforms

### AWS

1. Create an EC2 instance (t3.micro or higher)
2. Open ports 4001 (TCP) and 8080 (TCP) in Security Group
3. Connect to the instance via SSH
4. Install Node.js and dependencies:
   ```bash
   curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
   sudo apt-get install -y nodejs
   ```
5. Clone the repository and run the system:
   ```bash
   git clone <repository-url>
   cd p2p-decentralized-emulation/implementations/javascript
   npm install
   npm start
   ```

### Google Cloud Platform

1. Create a Compute Engine instance (e2-micro or higher)
2. Open ports 4001 and 8080 in the firewall
3. Connect to the instance via SSH
4. Install Node.js and dependencies:
   ```bash
   curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
   sudo apt-get install -y nodejs
   ```
5. Clone the repository and run the system:
   ```bash
   git clone <repository-url>
   cd p2p-decentralized-emulation/implementations/javascript
   npm install
   npm start
   ```

### Microsoft Azure

1. Create a virtual machine (B1S or higher)
2. Open ports 4001 and 8080 in the network security group
3. Connect to the virtual machine via SSH
4. Install Node.js and dependencies:
   ```bash
   curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
   sudo apt-get install -y nodejs
   ```
5. Clone the repository and run the system:
   ```bash
   git clone <repository-url>
   cd p2p-decentralized-emulation/implementations/javascript
   npm install
   npm start
   ```

### Heroku

1. Install Heroku CLI
2. Create a new application:
   ```bash
   heroku create
   ```
3. Add Node.js buildpack:
   ```bash
   heroku buildpacks:set heroku/nodejs
   ```
4. Deploy the application:
   ```bash
   git push heroku main
   ```

## Containerization

### Docker

To run in Docker:

```bash
cd implementations/javascript
docker build -t p2p-node .
docker run -p 4001:4001 -p 8080:8080 p2p-node
```

### Docker Compose

To run with Docker Compose:

```bash
cd implementations/javascript
docker-compose up -d
```

### Kubernetes

To deploy in Kubernetes:

1. Create a deployment:
   ```yaml
   apiVersion: apps/v1
   kind: Deployment
   metadata:
     name: p2p-node
   spec:
     replicas: 1
     selector:
       matchLabels:
         app: p2p-node
     template:
       metadata:
         labels:
           app: p2p-node
       spec:
         containers:
         - name: p2p-node
           image: p2p-node:latest
           ports:
           - containerPort: 4001
           - containerPort: 8080
   ```

2. Apply the configuration:
   ```bash
   kubectl apply -f deployment.yaml
   ```

## Monitoring and Logging

### Logging

The system automatically writes logs to stdout/stderr. To save logs, use:

```bash
npm start > app.log 2>&1 &
```

### Monitoring

To monitor node status, use:

```bash
# Check process status
ps aux | grep node

# Check open ports
netstat -tulpn | grep :4001
netstat -tulpn | grep :8080
```

## Backup

Node data is stored in the `data` directory. For backup:

```bash
# Create backup
tar -czf p2p-node-backup-$(date +%Y%m%d).tar.gz data

# Restore from backup
tar -xzf p2p-node-backup-*.tar.gz
```

## System Update

To update the system:

1. Stop the current node:
   ```bash
   npm stop
   ```

2. Get the latest changes:
   ```bash
   git pull
   ```

3. Update dependencies:
   ```bash
   npm install
   ```

4. Start the updated node:
   ```bash
   npm start
   ```