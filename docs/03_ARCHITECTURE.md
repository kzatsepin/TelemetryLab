# Архитектура и техническая карта

## 1. Общая схема

```text
telemetry-simulator (отдельный процесс)
        │
        │ UDP datagrams
        ▼
UdpReceiverWorker : QObject          [I/O QThread]
        │
        │ decoded immutable batches
        ▼
Processing / rolling statistics      [QThreadPool]
        │
        │ immutable results
        ▼
TelemetryService / models            [GUI thread]
        │
        ├── DeviceListModel
        ├── DeviceDetails / history facade
        ├── EventListModel
        └── Dashboard state
        │
        ▼
Qt Quick / QML + Qt Graphs           [GUI thread]

Export / historical analysis         [QtConcurrent + QFuture/QPromise]
Settings persistence                 [QSettings]
```

Главное правило проекта: **background code производит данные, GUI thread владеет presentation state и Qt models**.

## 2. Почему здесь три разных вида concurrency

| Механизм | Для чего | Почему |
|---|---|---|
| `QThread + QObject` | Долгоживущий network worker | socket и timers естественно работают через event loop. |
| `QThreadPool` | Короткие CPU-bound jobs | не нужен отдельный поток на каждую операцию; удобно ограничивать outstanding work. |
| `QtConcurrent + QFuture/QPromise` | Пользовательский export/analysis job | хорошо ложится на progress/cancel/result и `QFutureWatcher` в GUI. |

## 3. Компоненты

### Domain/value types

По возможности обычный C++ без `QObject`:

- `DeviceId` / `DeviceState`;
- `TelemetrySample`;
- `TelemetryBatch`;
- `DeviceMetrics`;
- `HistoryPoint`;
- `UserEvent`;
- `SourceConfig`;
- `ExportRequest`.

Передаваемые между потоками данные должны быть value-oriented и не ссылаться на изменяемую память другого потока.

### TelemetryService

Оркестратор на GUI thread:

- запускает/останавливает network thread;
- владеет моделями и состоянием приложения;
- принимает immutable результаты из worker/pool;
- переводит системные ошибки в пользовательские события;
- предоставляет стабильный API для QML.

### DeviceListModel

`QAbstractListModel` для экрана №2 и Dashboard summary.

Минимальные roles:

- id;
- name;
- group;
- status;
- temperature;
- humidity;
- voltage;
- signalLevel;
- lastUpdate.

### History store/facade

Хранит **ограниченную** историю показателей по устройствам. Она нужна экрану №4 и export snapshot.

Требования:

- верхняя граница количества точек/времени;
- GUI не читает mutable history worker-потока;
- статистика по большим диапазонам может считаться в pool;
- chart получает подготовленный диапазон, а не всю бесконечную историю.

### EventListModel

Отдельная пользовательская модель для экрана №5.

`UserEvent` минимум:

- timestamp;
- optional deviceId/sourceId;
- severity: info/warning/error;
- message/code.

**Не смешивать** с `QLoggingCategory`: diagnostic logs — для разработчика, `EventListModel` — для пользователя.

### SettingsService

`QObject`/facade над `QSettings`:

- theme;
- language;
- visual refresh interval;
- measurement units;
- default export path/format;
- source configs или ссылка на место их хранения.

QML работает с понятными properties/API, а не вызывает `QSettings` напрямую во многих файлах.

### ExportService

- на GUI thread формирует immutable snapshot;
- запускает QtConcurrent job;
- возвращает progress/state/error через `QFutureWatcher`/signals;
- никогда не передаёт `QAbstractItemModel` в background job.

## 4. Привязка экранов к backend

| Экран | Основной backend |
|---|---|
| 1 Dashboard | `TelemetryService` + summary properties + bounded activity history + последние `UserEvent` |
| 2 Устройства | `DeviceListModel` + proxy/filter layer в QML или C++ при необходимости |
| 3 Устройство | выбранный `DeviceState`/details facade + history/event filters |
| 4 Графики | bounded history + rolling statistics + Qt Graphs |
| 5 Журнал | `EventListModel` |
| 6 Настройки | `SettingsService` / `QSettings` |
| 7 Добавить источник | `SourceConfig` + source manager |
| 8 Экспорт | `ExportService` |
| 9 Ошибка подключения | state/error signal TelemetryService → QML Dialog/Popup + `UserEvent` |

## 5. Навигация QML

Для shell достаточно Qt Quick Controls:

- `ApplicationWindow`;
- постоянное боковое меню;
- `StackView` или эквивалент для основных страниц;
- `TabBar` для вкладок устройства;
- `Dialog`/`Popup` для №7, №8 и №9.

Не нужно создавать роутер-фреймворк или сложный MVVM framework ради учебного проекта.

Документация:

- https://doc.qt.io/qt-6/qtquickcontrols-index.html
- https://doc.qt.io/qt-6/qtquick-controls-qmlmodule.html

## 6. Графики

Используется **Qt Graphs 2D**:

- `GraphsView`;
- `LineSeries`;
- `ValueAxis` / `DateTimeAxis` при необходимости.

Зависимость появляется только тогда, когда начинается задача с экраном №4.

Документация:

- https://doc.qt.io/qt-6/qtgraphs-index.html
- https://doc.qt.io/qt-6/graphs-qml-2d.html
- https://doc.qt.io/qt-6/qml-qtgraphs-graphsview.html

## 7. Сетевой протокол core-версии

Core transport: UDP на localhost между simulator и app.

Пример минимальной JSON-схемы:

```json
{
  "protocolVersion": 1,
  "deviceId": "Device_001",
  "sequence": 42,
  "timestampMs": 1787590000000,
  "temperature": 23.4,
  "humidity": 45.0,
  "voltage": 3.71,
  "signalDbm": -67,
  "status": "online"
}
```

Правила:

- decoder — обычная тестируемая C++ функция;
- неизвестная/невалидная версия не валит приложение;
- sequence помогает видеть reorder/loss;
- packet size имеет разумный limit;
- invalid packets увеличивают metric и могут породить diagnostic log, но не ломают model.

## 8. TCP после core-плана

Экран №7 уже умеет выбрать `TCP Socket`, но реализацию TCP не надо смешивать с первым освоением UDP.

После задачи 15 можно добавить:

- `QTcpSocket`;
- состояния Connecting / Connected / Reconnecting / Error;
- `connected`, `disconnected`, `errorOccurred`;
- reconnect with bounded backoff;
- ручное Stop, которое отменяет reconnect;
- тот же UI журнала и экран №9 для ошибок.

Документация: https://doc.qt.io/qt-6/qtcpsocket.html

## 9. Технический logging и пользовательский журнал

Diagnostic categories минимум:

- `telemetry.network`;
- `telemetry.pipeline`;
- `telemetry.model`;
- `telemetry.export`;
- `telemetry.settings`.

Пользовательский журнал содержит только события, которые имеют смысл человеку. Нельзя отображать сырые `qDebug` строки как экран №5.

## 10. Метрики

Минимальный набор:

- received packets/sec;
- invalid/dropped packets;
- emitted batches/sec;
- average/max batch size;
- processing latency;
- outstanding pool jobs;
- model apply latency;
- GUI update rate;
- history size;
- export progress/duration.

## 11. Карта технологий по задачам

| Технология | Задача | Где применяется |
|---|---:|---|
| CMake / `qt_add_qml_module` | 0 | структура проекта |
| QObject / signals / properties | 1 | backend API |
| Qt Quick Controls / navigation | 2 | shell + Dashboard |
| `QAbstractListModel` | 3 | устройства |
| Qt Test / `QSignalSpy` / model tester | 4 | тесты |
| `QUdpSocket` / JSON | 5 | simulator + source |
| `QThread` / `moveToThread` | 6 | network worker |
| lifecycle / `deleteLater` | 7 | clean stop + errors |
| batching | 8 | live Dashboard/update pressure |
| `QThreadPool` / `QSemaphore` | 9 | stats + backpressure |
| Qt Graphs | 9 | экран графиков |
| QtConcurrent / QFuture / QPromise | 10 | export |
| `QLoggingCategory` + EventListModel | 11 | diagnostics + журнал |
| `QSettings` | 12 | настройки |
| QML Profiler / measurements | 13 | performance |
| async integration tests | 14 | races/failures |
| README / architecture rationale | 15 | портфолио |

## 12. Целевая структура репозитория

```text
telemetry-lab/
├── CMakeLists.txt
├── AGENTS.md
├── README.md
├── docs/
│   └── ui-reference.png
├── src/
│   ├── domain/
│   ├── core/
│   │   ├── models/
│   │   ├── services/
│   │   └── pipeline/
│   ├── app/
│   └── simulator/
├── qml/
│   ├── Main.qml
│   ├── pages/
│   │   ├── DashboardPage.qml
│   │   ├── DevicesPage.qml
│   │   ├── DeviceDetailsPage.qml
│   │   ├── DeviceGraphsPage.qml
│   │   ├── EventLogPage.qml
│   │   └── SettingsPage.qml
│   ├── dialogs/
│   │   ├── AddSourceDialog.qml
│   │   ├── ExportDialog.qml
│   │   └── ConnectionErrorDialog.qml
│   └── components/
└── tests/
    ├── unit/
    └── integration/
```

Не создавай все каталоги в задаче 0. Это **целевая** структура: папка появляется, когда появляется соответствующий компонент.

## 13. Опциональные расширения после core-плана

| Расширение | Технологии | Когда делать |
|---|---|---|
| TCP + reconnect | `QTcpSocket`, state/backoff | После core, чтобы довести экран №7 до вида референса. |
| Реальный system tray | platform/Qt tray integration | После settings core. |
| Реальный OS autostart | platform-specific integration | Только если хочется polished desktop UX. |
| Desktop notifications | platform notification API | После пользовательского event log. |
| Persistence history | Qt SQL / SQLite | Если нужна история между перезапусками. |
| QML interaction tests | Qt Quick Test | Если хочется тестировать UI interactions отдельно. |
| Widgets bridge | Qt Widgets Model/View | Для desktop/legacy вакансий. |
