// Пример использования интегрированного P2P узла с libp2p

import { createIntegratedNode } from './index.js'

/**
 * Пример системы событий для интеграции
 */
class EventSystem {
  constructor() {
    this.events = []
    this.handlers = []
  }
  
  /**
   * Добавление обработчика событий
   * @param {Function} handler - Обработчик событий
   */
  on(handler) {
    this.handlers.push(handler)
  }
  
  /**
   * Обработка события
   * @param {Object} event - Событие для обработки
   */
  processEvent(event) {
    console.log('Обработка события:', event)
    this.events.push(event)
    
    // Вызов всех обработчиков
    this.handlers.forEach(handler => {
      try {
        handler(event)
      } catch (error) {
        console.error('Ошибка обработки события:', error)
      }
    })
  }
  
  /**
   * Публикация события
   * @param {Object} event - Событие для публикации
   */
  async publishEvent(event) {
    console.log('Публикация события:', event)
    this.processEvent(event)
  }
  
  /**
   * Получение всех событий
   * @returns {Array} Список всех событий
   */
  getEvents() {
    return this.events
  }
}

/**
 * Основная функция примера
 */
async function main() {
  console.log('Запуск примера интеграции libp2p...')
  
  // Создание системы событий
  const eventSystem = new EventSystem()
  
  // Создание интегрированного узла
  const node = await createIntegratedNode({
    port: 3001,
    bootstrapList: [],
    enableDHT: false, // Отключено для локального тестирования
    enableMDNS: true,
    transports: ['tcp', 'websocket']
  })
  
  // Интеграция с системой событий
  node.integrateWithEventSystem(eventSystem)
  
  // Добавление обработчика событий
  eventSystem.on((event) => {
    console.log('Обработчик событий получил событие:', event)
  })
  
  // Подписка на тему событий
  await node.subscribeToTopic('test-topic', async (msg) => {
    try {
      const messageString = new TextDecoder().decode(msg.data)
      const message = JSON.parse(messageString)
      console.log('Получено сообщение из темы test-topic:', message)
    } catch (error) {
      console.error('Ошибка обработки сообщения:', error)
    }
  })
  
  // Запуск узла
  await node.start()
  
  // Публикация тестового события через систему событий
  setTimeout(async () => {
    const testEvent = {
      timestamp: Date.now(),
      type: 'test-event',
      payload: {
        message: 'Тестовое событие из интегрированного узла',
        nodeId: node.getPeerId()
      }
    }
    
    await eventSystem.publishEvent(testEvent)
  }, 2000)
  
  // Публикация тестового сообщения в тему
  setTimeout(async () => {
    const testMessage = {
      timestamp: Date.now(),
      type: 'test-message',
      payload: {
        message: 'Тестовое сообщение в тему',
        nodeId: node.getPeerId()
      }
    }
    
    await node.publishToTopic('test-topic', testMessage)
  }, 3000)
  
  // Отображение подключенных узлов каждые 5 секунд
  setInterval(() => {
    const peers = node.getConnectedPeers()
    console.log(`Подключенные узлы (${peers.length}):`, peers)
  }, 5000)
  
  // Обработка сигнала завершения
  process.on('SIGINT', async () => {
    console.log('
Получен сигнал завершения, остановка узла...')
    await node.stop()
    process.exit(0)
  })
}

// Запуск примера
if (import.meta.url === `file://${process.argv[1]}`) {
  main().catch(console.error)
}

export default main