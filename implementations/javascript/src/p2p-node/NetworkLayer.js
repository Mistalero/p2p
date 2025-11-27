// NetworkLayer.js
// Сетевой слой для P2P-ноды

import net from 'net';
import EventEmitter from 'events';

class NetworkLayer extends EventEmitter {
  constructor(port, eventLog, keyManager) {
    super();
    this.port = port || 3000;
    this.eventLog = eventLog;
    this.keyManager = keyManager;
    this.server = null;
    this.connections = new Map();
  }

  // Запуск сервера
  startServer() {
    this.server = net.createServer((socket) => {
      this.handleConnection(socket);
    });

    this.server.listen(this.port, () => {
      console.log(`P2P node listening on port ${this.port}`);
    });

    this.server.on('error', (err) => {
      console.error('Server error:', err);
    });
  }

  // Обработка подключения
  handleConnection(socket) {
    const clientId = `${socket.remoteAddress}:${socket.remotePort}`;
    console.log(`New connection from ${clientId}`);
    
    this.connections.set(clientId, socket);
    
    // Отправляем все наши события новому подключению
    this.sendAllEvents(socket);
    
    // Обрабатываем входящие данные
    let buffer = '';
    socket.on('data', (data) => {
      buffer += data.toString();
      
      // Обрабатываем полные сообщения (разделены 
)
      let newlineIndex;
      while ((newlineIndex = buffer.indexOf('
')) !== -1) {
        const message = buffer.substring(0, newlineIndex);
        buffer = buffer.substring(newlineIndex + 1);
        
        try {
          this.handleMessage(socket, JSON.parse(message));
        } catch (error) {
          console.error('Error parsing message:', error);
        }
      }
    });
    
    socket.on('close', () => {
      console.log(`Connection closed: ${clientId}`);
      this.connections.delete(clientId);
    });
    
    socket.on('error', (err) => {
      console.error(`Socket error for ${clientId}:`, err);
      this.connections.delete(clientId);
    });
  }

  // Отправка всех событий клиенту
  sendAllEvents(socket) {
    const events = this.eventLog.getEvents();
    const message = {
      type: 'event_sync',
      events: events
    };
    
    this.sendMessage(socket, message);
  }

  // Обработка входящего сообщения
  handleMessage(socket, message) {
    switch (message.type) {
      case 'event_sync':
        this.handleEventSync(message);
        break;
      case 'new_event':
        this.handleNewEvent(message);
        break;
      default:
        console.warn(`Unknown message type: ${message.type}`);
    }
  }

  // Обработка синхронизации событий
  handleEventSync(message) {
    if (message.events && Array.isArray(message.events)) {
      const newEventsCount = this.eventLog.mergeEvents(message.events);
      console.log(`Merged ${newEventsCount} new events from peer`);
      
      // Уведомляем подписчиков о новых событиях
      if (newEventsCount > 0) {
        this.emit('events_updated');
      }
    }
  }

  // Обработка нового события
  handleNewEvent(message) {
    if (message.event) {
      // Проверяем подпись события
      const event = message.event;
      const signature = message.signature;
      const publicKey = message.publicKey;
      
      // Восстанавливаем оригинальные данные для проверки подписи
      const dataToVerify = JSON.stringify({
        timestamp: event.timestamp,
        author: event.author,
        type: event.type,
        payload: event.payload
      });
      
      if (this.keyManager.verifySignature(dataToVerify, signature, publicKey)) {
        if (this.eventLog.addEvent(event)) {
          console.log('Added new event from peer');
          this.emit('events_updated');
          
          // Рассылаем событие всем подключенным узлам
          this.broadcastEvent(event, signature, publicKey);
        }
      } else {
        console.warn('Invalid signature for event');
      }
    }
  }

  // Отправка сообщения клиенту
  sendMessage(socket, message) {
    try {
      socket.write(JSON.stringify(message) + '
');
    } catch (error) {
      console.error('Error sending message:', error);
    }
  }

  // Рассылка события всем подключенным узлам
  broadcastEvent(event, signature, publicKey) {
    const message = {
      type: 'new_event',
      event: event,
      signature: signature,
      publicKey: publicKey
    };
    
    for (const [clientId, socket] of this.connections) {
      this.sendMessage(socket, message);
    }
  }

  // Подключение к другой ноде
  connectToPeer(host, port) {
    const socket = new net.Socket();
    const clientId = `${host}:${port}`;
    
    socket.connect(port, host, () => {
      console.log(`Connected to peer ${clientId}`);
      this.connections.set(clientId, socket);
      
      // Обрабатываем входящие данные
      let buffer = '';
      socket.on('data', (data) => {
        buffer += data.toString();
        
        // Обрабатываем полные сообщения (разделены 
)
        let newlineIndex;
        while ((newlineIndex = buffer.indexOf('
')) !== -1) {
          const message = buffer.substring(0, newlineIndex);
          buffer = buffer.substring(newlineIndex + 1);
          
          try {
            this.handleMessage(socket, JSON.parse(message));
          } catch (error) {
            console.error('Error parsing message:', error);
          }
        }
      });
      
      socket.on('close', () => {
        console.log(`Connection to peer closed: ${clientId}`);
        this.connections.delete(clientId);
      });
      
      socket.on('error', (err) => {
        console.error(`Socket error for peer ${clientId}:`, err);
        this.connections.delete(clientId);
      });
    });
    
    socket.on('error', (err) => {
      console.error(`Failed to connect to peer ${clientId}:`, err);
    });
  }

  // Остановка сервера
  stopServer() {
    if (this.server) {
      this.server.close(() => {
        console.log('Server closed');
      });
    }
    
    // Закрываем все соединения
    for (const [clientId, socket] of this.connections) {
      socket.destroy();
    }
    
    this.connections.clear();
  }
}

export default NetworkLayer;