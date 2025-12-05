# Интеграция libp2p в P2P-систему

## Обзор

Этот модуль предоставляет интеграцию libp2p в существующую P2P-архитектуру, обеспечивая лучшую масштабируемость, отказоустойчивость и возможности обнаружения узлов.

## Возможности

### 1. Улучшенное обнаружение узлов
- DHT для глобального обнаружения узлов
- mDNS для локального обнаружения
- Bootstrap списки для начального подключения

### 2. Надежная передача сообщений
- Поддержка нескольких транспортов (TCP, WebSocket, WebRTC)
- Встроенная шифрование (Noise)
- Мультиплексирование потоков (Yamux)

### 3. Масштабируемая широковещательная рассылка
- GossipSub для эффективной рассылки сообщений
- Поддержка тем и подписок
- Контроль дублирования сообщений

### 4. Интеграция с существующими компонентами
- Совместимость с существующей системой идентификации (DID)
- Интеграция с системой событий
- Поддержка IPLD для управления состоянием

## Установка

```bash
npm install
```

## Использование

### Создание интегрированного узла

```javascript
import { createIntegratedNode } from './index.js'

const node = await createIntegratedNode({
  port: 3000,
  bootstrapList: ['/ip4/127.0.0.1/tcp/3000/p2p/Qm...'],
  enableDHT: true,
  enableMDNS: true,
  transports: ['tcp', 'websocket']
})

await node.start()
```

### Работа с PubSub

```javascript
// Подписка на тему
await node.subscribeToTopic('chat-messages', (msg) => {
  const message = JSON.parse(new TextDecoder().decode(msg.data))
  console.log('Получено сообщение:', message)
})

// Публикация сообщения
const message = {
  timestamp: Date.now(),
  author: node.getPeerId(),
  content: 'Привет, мир!'
}

await node.publishToTopic('chat-messages', message)
```

### Интеграция с системой событий

```javascript
// Интеграция с существующей системой событий
node.integrateWithEventSystem(eventSystem)

// События теперь будут автоматически публиковаться через PubSub
eventSystem.publishEvent({
  timestamp: Date.now(),
  type: 'user_action',
  payload: { action: 'click', element: 'button1' }
})
```

## Архитектура

### Слои интеграции

```
┌─────────────────────────────────────────────────┐
│              Application Layer                   │
│  (Business Logic, Event Processing)            │
├─────────────────────────────────────────────────┤
│           libp2p Integration Layer               │
│  (Peer Discovery, PubSub, Content Routing)      │
├─────────────────────────────────────────────────┤
│         Existing P2P Implementation           │
│  (Identity, State Sync, Transport, Events)       │
├─────────────────────────────────────────────────┤
│            Network Interface Layer              │
│  (TCP, UDP, WebSockets, WebRTC, QUIC)          │
└─────────────────────────────────────────────────┘
```

### Компоненты интеграции

1. **Identity Integration** - Мост между DID и PeerID
2. **Event System Integration** - Интеграция с существующей системой событий
3. **State Management Integration** - Интеграция с IPLD для управления состоянием
4. **Transport Integration** - Гибридный транспортный слой

## Конфигурация

### Опции создания узла

| Опция | Тип | По умолчанию | Описание |
|-------|-----|-------------|---------|
| `port` | number | 3000 | Порт для прослушивания |
| `bootstrapList` | Array<string> | [] | Список bootstrap узлов |
| `enableDHT` | boolean | true | Включить DHT для обнаружения узлов |
| `enableMDNS` | boolean | true | Включить mDNS для локального обнаружения |
| `transports` | Array<string> | ['tcp', 'websocket'] | Список транспортов для использования |

## Примеры

### Базовый пример

```javascript
import { createIntegratedNode } from './index.js'

async function main() {
  const node = await createIntegratedNode({
    port: 3001,
    enableDHT: false,
    enableMDNS: true
  })
  
  await node.start()
  
  // Подписка на тему
  await node.subscribeToTopic('test-topic', (msg) => {
    console.log('Получено сообщение:', msg.data.toString())
  })
  
  // Публикация сообщения
  await node.publishToTopic('test-topic', 'Привет, libp2p!')
}

main().catch(console.error)
```

### Интеграция с системой событий

```javascript
import { createIntegratedNode } from './index.js'

class EventSystem {
  constructor() {
    this.events = []
  }
  
  async publishEvent(event) {
    this.events.push(event)
    console.log('Событие опубликовано:', event)
  }
}

async function main() {
  const eventSystem = new EventSystem()
  const node = await createIntegratedNode({ port: 3001 })
  
  // Интеграция с системой событий
  node.integrateWithEventSystem(eventSystem)
  
  await node.start()
  
  // Публикация события через интегрированную систему
  await eventSystem.publishEvent({
    type: 'test',
    payload: 'Тестовое событие'
  })
}

main().catch(console.error)
```

## Тестирование

```bash
npm test
```

## Лицензия

MIT