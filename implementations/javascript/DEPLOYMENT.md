# Развертывание P2P Decentralized Emulation System

## Облачные платформы

### AWS

1. Создайте EC2 инстанс (t3.micro или выше)
2. Откройте порты 4001 (TCP) и 8080 (TCP) в Security Group
3. Подключитесь к инстансу через SSH
4. Установите Node.js и зависимости:
   ```bash
   curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
   sudo apt-get install -y nodejs
   ```
5. Клонируйте репозиторий и запустите систему:
   ```bash
   git clone <repository-url>
   cd p2p-decentralized-emulation/implementations/javascript
   npm install
   npm start
   ```

### Google Cloud Platform

1. Создайте Compute Engine инстанс (e2-micro или выше)
2. Откройте порты 4001 и 8080 в брандмауэре
3. Подключитесь к инстансу через SSH
4. Установите Node.js и зависимости:
   ```bash
   curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
   sudo apt-get install -y nodejs
   ```
5. Клонируйте репозиторий и запустите систему:
   ```bash
   git clone <repository-url>
   cd p2p-decentralized-emulation/implementations/javascript
   npm install
   npm start
   ```

### Microsoft Azure

1. Создайте виртуальную машину (B1S или выше)
2. Откройте порты 4001 и 8080 в группе безопасности сети
3. Подключитесь к виртуальной машине через SSH
4. Установите Node.js и зависимости:
   ```bash
   curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
   sudo apt-get install -y nodejs
   ```
5. Клонируйте репозиторий и запустите систему:
   ```bash
   git clone <repository-url>
   cd p2p-decentralized-emulation/implementations/javascript
   npm install
   npm start
   ```

### Heroku

1. Установите Heroku CLI
2. Создайте новое приложение:
   ```bash
   heroku create
   ```
3. Добавьте buildpack для Node.js:
   ```bash
   heroku buildpacks:set heroku/nodejs
   ```
4. Разверните приложение:
   ```bash
   git push heroku main
   ```

## Контейнеризация

### Docker

Для запуска в Docker:

```bash
cd implementations/javascript
docker build -t p2p-node .
docker run -p 4001:4001 -p 8080:8080 p2p-node
```

### Docker Compose

Для запуска с помощью Docker Compose:

```bash
cd implementations/javascript
docker-compose up -d
```

### Kubernetes

Для развертывания в Kubernetes:

1. Создайте deployment:
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

2. Примените конфигурацию:
   ```bash
   kubectl apply -f deployment.yaml
   ```

## Мониторинг и логирование

### Логирование

Система автоматически записывает логи в stdout/stderr. Для сохранения логов используйте:

```bash
npm start > app.log 2>&1 &
```

### Мониторинг

Для мониторинга состояния узла используйте:

```bash
# Проверка состояния процесса
ps aux | grep node

# Проверка открытых портов
netstat -tulpn | grep :4001
netstat -tulpn | grep :8080
```

## Резервное копирование

Данные узла хранятся в директории `data`. Для резервного копирования:

```bash
# Создание резервной копии
tar -czf p2p-node-backup-$(date +%Y%m%d).tar.gz data

# Восстановление из резервной копии
tar -xzf p2p-node-backup-*.tar.gz
```

## Обновление системы

Для обновления системы:

1. Остановите текущий узел:
   ```bash
   npm stop
   ```

2. Получите последние изменения:
   ```bash
   git pull
   ```

3. Обновите зависимости:
   ```bash
   npm install
   ```

4. Запустите обновленный узел:
   ```bash
   npm start
   ```