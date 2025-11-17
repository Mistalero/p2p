# Автоматический деплой P2P узла в Yandex Cloud

Этот скрипт автоматизирует процесс деплоя P2P узла в Yandex Cloud через Container Registry и Compute Cloud.

## Требования

Перед запуском скрипта убедитесь, что у вас установлены:

1. [Yandex Cloud CLI](https://cloud.yandex.ru/docs/cli/quickstart)
2. Docker
3. Доступ к вашему облаку Yandex Cloud с необходимыми правами

## Подготовка

1. Авторизуйтесь в Yandex Cloud CLI:
   ```bash
   yc init
   ```

2. Убедитесь, что вы находитесь в правильном каталоге (folder)

## Использование

1. Сделайте скрипт исполняемым:
   ```bash
   chmod +x deploy/yandex-cloud-deploy.sh
   ```

2. Запустите скрипт:
   ```bash
   ./deploy/yandex-cloud-deploy.sh
   ```

## Что делает скрипт

1. Проверяет наличие необходимых инструментов (yc, docker)
2. Собирает Docker-образ приложения
3. Настраивает Docker для работы с Yandex Container Registry
4. Создает Container Registry (если еще не существует)
5. Загружает образ в Container Registry
6. Создает виртуальную машину с Container Optimized Image
7. Настраивает виртуальную машину для запуска контейнера с приложением

## Параметры скрипта

Скрипт использует следующие параметры по умолчанию:
- Название реестра: `p2p-registry`
- Название ВМ: `p2p-node`
- Зона доступности: `ru-central1-a`

Эти параметры можно изменить в самом скрипте при необходимости.

## После деплоя

После успешного выполнения скрипта ваш P2P узел будет запущен в облаке. Вы можете проверить его статус с помощью:

```bash
yc compute instance get --name p2p-node
```

Для подключения к узлу по SSH используйте:

```bash
yc compute instance ssh p2p-node
```

## Мониторинг

Для мониторинга состояния узла можно использовать:

```bash
yc compute instance get serial-port-output --name p2p-node
```

Это покажет логи загрузки и работы узла.