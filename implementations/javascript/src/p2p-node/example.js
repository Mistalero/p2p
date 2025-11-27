// example.js
// Пример использования P2P-ноды

import P2PNode from './P2PNode.js';

// Получение порта из аргументов командной строки или использование порта по умолчанию
const port = process.argv[2] ? parseInt(process.argv[2]) : 3000;

// Создание ноды
const node = new P2PNode({
  port: port,
  logFilePath: `node_${port}_events.log`
});

// Запуск ноды
node.start();

console.log(`P2P node started on port ${port}`);

// Добавление примеров событий через 2 секунды
setTimeout(() => {
  console.log('Adding sample events...');
  
  node.addEvent('user_action', {
    action: 'click',
    element: 'button1',
    timestamp: Date.now()
  });
  
  node.addEvent('system_event', {
    type: 'startup',
    version: '1.0.0'
  });
  
  console.log('Current events:', node.getEvents());
  
  // Вывод информации о ноде
  console.log('Node info:', node.getNodeInfo());
}, 2000);

// Подключение к другой ноде (если указаны аргументы)
if (process.argv[3] && process.argv[4]) {
  const host = process.argv[3];
  const peerPort = parseInt(process.argv[4]);
  console.log(`Connecting to peer at ${host}:${peerPort}`);
  node.connectToPeer(host, peerPort);
}

// Обработка сигнала завершения
process.on('SIGINT', () => {
  console.log('Shutting down node...');
  node.stop();
  process.exit(0);
});