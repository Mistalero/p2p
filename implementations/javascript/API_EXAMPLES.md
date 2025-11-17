# Примеры использования API P2P Decentralized Emulation System

## Основные концепции

Система предоставляет несколько способов взаимодействия:

1. **Программный API** - для интеграции в другие приложения
2. **PubSub сообщения** - для обмена данными между узлами
3. **Семантическая адаптация** - для совместимости с другими протоколами

## Примеры программного API

### Создание и управление CRDT

```javascript
import { StateSyncLayer } from './src/index.js';

// Создание слоя синхронизации состояния
const stateSyncLayer = new StateSyncLayer();

// Создание LWWRegister для хранения позиции игрока
const positionRegister = stateSyncLayer.createCRDT('player-position', 'LWWRegister', { x: 0, y: 0, z: 0 });

// Создание ORSet для инвентаря игрока
const inventorySet = stateSyncLayer.createCRDT('player-inventory', 'ORSet', ['sword', 'shield']);

// Обновление позиции
positionRegister.set({ x: 10, y: 20, z: 0 }, stateSyncLayer.getNodeId());

// Добавление предмета в инвентарь
inventorySet.add('potion', stateSyncLayer.getNodeId());

// Получение текущих значений
console.log('Position:', positionRegister.value());
console.log('Inventory:', inventorySet.value());
```

### Работа с транспортным слоем

```javascript
import TransportLayer from './src/TransportLayer.js';

// Создание транспортного слоя
const transportLayer = new TransportLayer(stateSyncLayer);

// Инициализация
await transportLayer.initialize();

// Подписка на топик
await transportLayer.subscribeToTopic('game-state');

// Публикация сообщения
await transportLayer.publishMessage('game-state', {
  type: 'player_move',
  playerId: stateSyncLayer.getNodeId(),
  position: positionRegister.value(),
  timestamp: Date.now()
});

// Отправка прямого сообщения другому узлу
// (предполагается, что peerId известен)
// await transportLayer.sendDirectMessage(peerId, message);
```

### Семантическая адаптация

```javascript
import { SemanticAdapter } from './src/index.js';

// Создание семантического адаптера
const semanticAdapter = new SemanticAdapter(stateSyncLayer);

// Конвертация в формат ActivityPub
const positionActivityPub = semanticAdapter.crdtToActivityPub(positionRegister);
console.log('ActivityPub format:', JSON.stringify(positionActivityPub, null, 2));

// Конвертация в событие Nostr
const positionNostrEvent = semanticAdapter.crdtToNostrEvent(positionRegister, 'wss://relay.example.com');
console.log('Nostr event:', JSON.stringify(positionNostrEvent, null, 2));

// Сериализация для IPFS/IPLD
const positionIPLD = semanticAdapter.serializeForIPFS(positionRegister);
console.log('IPLD format:', JSON.stringify(positionIPLD, null, 2));
```

## Примеры PubSub сообщений

### Синхронизация состояния

```javascript
// Отправка состояния другим узлам
const stateMessage = {
  type: 'state_sync',
  sender: stateSyncLayer.getNodeId(),
  state: stateSyncLayer.serializeState(),
  timestamp: Date.now()
};

await transportLayer.publishMessage('p2p-state-sync', stateMessage);

// Обработка входящих сообщений о состоянии
transportLayer.handleStateSyncMessage = (message) => {
  // Десериализация и слияние состояния
  stateSyncLayer.deserializeState(message.state);
  console.log('State synchronized from', message.sender);
};
```

### Идентификация узлов

```javascript
// Отправка вызова для проверки идентичности
const challenge = Math.random().toString(36).substring(2, 15);
const challengeMessage = {
  type: 'identity_challenge',
  sender: stateSyncLayer.getNodeId(),
  challenge: challenge,
  timestamp: Date.now()
};

await transportLayer.publishMessage('p2p-identity', challengeMessage);

// Обработка ответа на вызов
transportLayer.handleIdentityResponse = (message) => {
  // Проверка подписи (в реальной реализации)
  // verifySignature(message.signature, message.challenge, message.sender);
  console.log('Identity verified for node', message.sender);
};
```

## Примеры интеграции с внешними протоколами

### Интеграция с ActivityPub

```javascript
// Создание ActivityPub объекта из CRDT
const activityPubObject = semanticAdapter.crdtToActivityPub(positionRegister);

// Публикация в ActivityPub совместимую систему
// (предполагается наличие HTTP клиента)
fetch('https://activitypub.example.com/inbox', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/activity+json'
  },
  body: JSON.stringify(activityPubObject)
});
```

### Интеграция с Nostr

```javascript
// Создание Nostr события из CRDT
const nostrEvent = semanticAdapter.crdtToNostrEvent(positionRegister, 'wss://relay.example.com');

// Публикация в Nostr релей
// (предполагается наличие Nostr клиента)
const relay = new WebSocket('wss://relay.example.com');
relay.onopen = () => {
  relay.send(JSON.stringify(['EVENT', nostrEvent]));
};
```

### Интеграция с IPFS

```javascript
// Сериализация CRDT для IPFS
const ipldObject = semanticAdapter.serializeForIPFS(positionRegister);

// Сохранение в IPFS
// (предполагается наличие IPFS клиента)
const cid = await ipfs.add(JSON.stringify(ipldObject));
console.log('CID:', cid.toString());
```

## Примеры расширения функциональности

### Добавление нового типа CRDT

```javascript
// Создание нового типа CRDT (например, счетчик)
import CRDT from './src/state-sync/CRDT.js';

class Counter extends CRDT {
  constructor(id, nodeId, initialValue = 0) {
    super(id, nodeId);
    this._value = initialValue;
  }

  increment(by = 1, nodeId) {
    this._value += by;
    this.nodeId = nodeId;
    this.timestamp = Date.now();
  }

  value() {
    return this._value;
  }

  applyOperation(operation) {
    if (operation.type === 'increment' && operation.timestamp > this.timestamp) {
      this._value += operation.by;
      this.nodeId = operation.nodeId;
      this.timestamp = operation.timestamp;
    }
  }

  merge(other) {
    if (other.timestamp > this.timestamp) {
      this._value = other._value;
      this.nodeId = other.nodeId;
      this.timestamp = other.timestamp;
    }
  }

  toJSON() {
    return {
      ...super.toJSON(),
      value: this._value
    };
  }

  static fromJSON(json) {
    const counter = new Counter(json.id, json.nodeId, json.value);
    counter.timestamp = json.timestamp;
    return counter;
  }
}

// Регистрация нового типа в StateSyncLayer
// (в реальной реализации потребуется модификация StateSyncLayer)
```

### Добавление нового обработчика сообщений

```javascript
// Расширение TransportLayer новым обработчиком
class ExtendedTransportLayer extends TransportLayer {
  constructor(stateSyncLayer) {
    super(stateSyncLayer);
    this.customHandlers = new Map();
  }

  // Добавление пользовательского обработчика
  addCustomHandler(messageType, handler) {
    this.customHandlers.set(messageType, handler);
  }

  // Расширение обработчика PubSub сообщений
  handlePubsubMessage(evt) {
    try {
      const message = JSON.parse(new TextDecoder().decode(evt.detail.data));
      
      // Проверка пользовательских обработчиков
      if (this.customHandlers.has(message.type)) {
        this.customHandlers.get(message.type)(message);
        return;
      }
      
      // Обработка стандартных сообщений
      super.handlePubsubMessage(evt);
    } catch (error) {
      console.error('Error handling pubsub message:', error);
    }
  }
}

// Использование расширенного транспортного слоя
const extendedTransportLayer = new ExtendedTransportLayer(stateSyncLayer);

// Добавление пользовательского обработчика
extendedTransportLayer.addCustomHandler('custom_message', (message) => {
  console.log('Received custom message:', message);
  // Обработка пользовательского сообщения
});
```

## Примеры использования в реальных сценариях

### Создание децентрализованной игры

```javascript
// Инициализация игрового узла
const gameState = {
  players: stateSyncLayer.createCRDT('players', 'ORSet', []),
  playerPositions: new Map(),
  gameObjects: stateSyncLayer.createCRDT('objects', 'ORSet', [])
};

// Добавление игрока
function addPlayer(playerId) {
  gameState.players.add(playerId, stateSyncLayer.getNodeId());
  gameState.playerPositions.set(playerId, 
    stateSyncLayer.createCRDT(`position-${playerId}`, 'LWWRegister', { x: 0, y: 0, z: 0 })
  );
}

// Обновление позиции игрока
function updatePlayerPosition(playerId, position) {
  const positionRegister = gameState.playerPositions.get(playerId);
  if (positionRegister) {
    positionRegister.set(position, stateSyncLayer.getNodeId());
  }
}

// Синхронизация игрового состояния
setInterval(() => {
  const gameStateMessage = {
    type: 'game_state',
    sender: stateSyncLayer.getNodeId(),
    state: {
      players: gameState.players.value(),
      positions: Array.from(gameState.playerPositions.entries()).reduce((acc, [id, reg]) => {
        acc[id] = reg.value();
        return acc;
      }, {}),
      objects: gameState.gameObjects.value()
    },
    timestamp: Date.now()
  };
  
  transportLayer.publishMessage('game-state', gameStateMessage);
}, 1000);
```

Эти примеры демонстрируют основные способы использования P2P Decentralized Emulation System. Система предоставляет гибкий и расширяемый API для создания децентрализованных приложений с возможностью синхронизации состояния, идентификации узлов и интероперабельности с другими протоколами.