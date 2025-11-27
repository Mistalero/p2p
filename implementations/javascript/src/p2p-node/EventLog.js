// EventLog.js
// Работа с логом событий для P2P-ноды

import fs from 'fs';
import path from 'path';

class EventLog {
  constructor(logFilePath) {
    this.logFilePath = logFilePath || 'events.log';
    this.events = [];
    this.loadEvents();
  }

  // Загрузка событий из файла
  loadEvents() {
    try {
      if (fs.existsSync(this.logFilePath)) {
        const data = fs.readFileSync(this.logFilePath, 'utf8');
        if (data) {
          this.events = data.split('
')
            .filter(line => line.trim() !== '')
            .map(line => JSON.parse(line));
        }
      }
    } catch (error) {
      console.error('Error loading events:', error);
      this.events = [];
    }
  }

  // Сохранение событий в файл
  saveEvents() {
    try {
      const data = this.events.map(event => JSON.stringify(event)).join('
');
      fs.writeFileSync(this.logFilePath, data);
    } catch (error) {
      console.error('Error saving events:', error);
    }
  }

  // Добавление события
  addEvent(event) {
    // Проверяем, есть ли уже такое событие
    const exists = this.events.some(e => 
      e.timestamp === event.timestamp && 
      e.author === event.author && 
      e.type === event.type && 
      JSON.stringify(e.payload) === JSON.stringify(event.payload)
    );
    
    if (!exists) {
      this.events.push(event);
      this.saveEvents();
      return true;
    }
    
    return false;
  }

  // Получение всех событий
  getEvents() {
    return [...this.events];
  }

  // Получение событий после определенного времени
  getEventsAfter(timestamp) {
    return this.events.filter(event => event.timestamp > timestamp);
  }

  // Объединение с другим логом событий
  mergeEvents(otherEvents) {
    let newEventsCount = 0;
    
    otherEvents.forEach(event => {
      if (this.addEvent(event)) {
        newEventsCount++;
      }
    });
    
    return newEventsCount;
  }

  // Получение последнего времени события
  getLastEventTime() {
    if (this.events.length === 0) {
      return 0;
    }
    
    return Math.max(...this.events.map(event => event.timestamp));
  }
}

export default EventLog;