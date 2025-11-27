// P2PNode.js
// Основной класс P2P-ноды

import KeyManager from './KeyManager.js';
import EventLog from './EventLog.js';
import NetworkLayer from './NetworkLayer.js';

class P2PNode {
  constructor(options = {}) {
    this.port = options.port || 3000;
    this.logFilePath = options.logFilePath || 'events.log';
    
    // Инициализация компонентов
    this.keyManager = new KeyManager();
    this.eventLog = new EventLog(this.logFilePath);
    this.networkLayer = new NetworkLayer(this.port, this.eventLog, this.keyManager);
    
    // Генерация ключей при запуске
    this.keyManager.generateKeyPair();
  }
  
  // Запуск ноды
  start() {
    console.log('Starting P2P node...');
    
    // Запуск сетевого слоя
    this.networkLayer.startServer();
    
    // Подписка на обновления событий
    this.networkLayer.on('events_updated', () => {
      console.log('Events updated from network');
    });
    
    console.log('P2P node started successfully');
  }
  
  // Остановка ноды
  stop() {
    console.log('Stopping P2P node...');
    this.networkLayer.stopServer();
    console.log('P2P node stopped');
  }
  
  // Добавление нового события
  addEvent(type, payload) {
    const event = {
      timestamp: Date.now(),
      author: this.keyManager.getPublicKey(),
      type: type,
      payload: payload
    };
    
    // Подписываем событие
    const dataToSign = JSON.stringify({
      timestamp: event.timestamp,
      author: event.author,
      type: event.type,
      payload: event.payload
    });
    
    const signature = this.keyManager.signData(dataToSign);
    
    // Добавляем событие в локальный лог
    if (this.eventLog.addEvent(event)) {
      console.log('New event added locally');
      
      // Рассылаем событие всем подключенным узлам
      this.networkLayer.broadcastEvent(event, signature, this.keyManager.getPublicKey());
      
      return event;
    }
    
    return null;
  }
  
  // Получение всех событий
  getEvents() {
    return this.eventLog.getEvents();
  }
  
  // Подключение к другой ноде
  connectToPeer(host, port) {
    console.log(`Connecting to peer at ${host}:${port}`);
    this.networkLayer.connectToPeer(host, port);
  }
  
  // Получение информации о ноде
  getNodeInfo() {
    return {
      publicKey: this.keyManager.getPublicKey(),
      port: this.port,
      eventCount: this.eventLog.getEvents().length,
      connectionCount: this.networkLayer.connections.size
    };
  }
}

export default P2PNode;