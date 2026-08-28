# Задачи — проходить строго по порядку

## Как читать этот файл

На каждой задаче:

1. прочитай только её раздел «Технологии» и указанные ссылки;
2. реализуй задачу сам;
3. запусти tests/build;
4. отдай diff Codex на review only;
5. исправь findings сам;
6. переходи дальше только после Definition of Done.

Полное пользовательское ТЗ экранов находится в [`02_PROJECT_SPEC.md`](./02_PROJECT_SPEC.md). Если в задаче написано «экран №4», не придумывай его заново — открой соответствующий раздел ТЗ.

---

## 0. Bootstrap: CMake + минимальное Qt Quick приложение

**Связанные экраны макета:** Нет обязательного экрана: только технический skeleton.

**Результат задачи:** репозиторий собирается из командной строки; есть GUI target, core library, simulator target и пустой test target.

Технологии, которые нужно понять перед кодом

- Modern CMake targets: find_package, target_link_libraries, add_subdirectory.

- qt_standard_project_setup(), qt_add_executable(), qt_add_qml_module().

- Разница между QGuiApplication и QCoreApplication на уровне назначения, без глубокого изучения internals.

Прочитать только это, затем сразу писать

- [Getting started with CMake](https://doc.qt.io/qt-6/cmake-get-started.html) — прочитай basic target setup и structuring projects

- [Building a QML application](https://doc.qt.io/qt-6/cmake-build-qml-application.html) — нужны qt_add_executable + qt_add_qml_module

- [Writing QML Modules](https://doc.qt.io/qt-6/qtqml-writing-a-module.html) — зачем QML module и как в него попадут C++ types

Что реализовать самостоятельно

1. Создай top-level CMakeLists.txt и отдельные targets: telemetry_core (library), telemetry_app, telemetry_simulator, telemetry_tests.

2. В telemetry_app подними QGuiApplication + QQmlApplicationEngine и Main.qml с одним окном/текстом.

3. В simulator пока сделай QCoreApplication, который просто стартует и завершает работу корректно.

4. Подключи CTest/add_test для будущих тестов — тест пока может быть минимальным smoke test.

Не распыляться сейчас

- Не изучай qmake и .pro-файлы.

- Не подключай Conan/vcpkg, если Qt уже установлен обычным способом.

- Не делай CI, packaging, installers и красивый UI.

- Не добавляй Networking/Concurrent «на будущее» — зависимость появляется вместе с задачей, где она нужна.

Что попросить Codex проверить именно в этой задаче

- Target-level зависимости: нет ли случайных transitive dependencies и глобальных include paths.

- Корректно ли создан QML module; нет ли ручного копирования QML файлов туда, куда CMake уже умеет.

- Можно ли собрать каждый executable независимо через declared targets.

Definition of Done

- [ ] Чистый configure/build из новой build-директории.

- [ ] telemetry_app запускает окно.

- [ ] ctest видит хотя бы один test target.

- [ ] Нет Qt API в domain-коде, если он пока не нужен.

После задачи ты должен уметь объяснить без документации

- Почему target_link_libraries лучше, чем глобальные include/link настройки?

- Зачем qt_add_qml_module, а не просто положить .qml рядом с exe?

- Какова роль moc и почему CMake должен знать о QObject/QML types?

## 1. QObject, ownership, meta-object и signals/slots

**Связанные экраны макета:** №1 Dashboard — backend-properties и базовое состояние без финального UI.

**Результат задачи:** появляется TelemetryService, который пока симулирует состояние в GUI thread и позволяет увидеть базовые правила Qt object model без многопоточности.

Технологии, которые нужно понять перед кодом

- QObject и parent-child ownership.

- Q_OBJECT, meta-object compiler (moc), signals/slots.

- Q_PROPERTY + NOTIFY.

- QTimer как event-driven источник событий в текущем потоке.

Прочитать только это, затем сразу писать

- [QObject](https://doc.qt.io/qt-6/qobject.html) — прочитай ownership/thread affinity sections и API around signals

- [The Meta-Object System](https://doc.qt.io/qt-6/metaobjects.html) — Q_OBJECT, moc, runtime metadata

- [Object Trees & Ownership](https://doc.qt.io/qt-6/objecttrees.html) — parent deletes children; важные исключения со stack objects

- [Signals & Slots](https://doc.qt.io/qt-6/signalsandslots.html) — modern function-pointer connect syntax и semantics

- [The Property System](https://doc.qt.io/qt-6/properties.html) — Q_PROPERTY/READ/NOTIFY

Что реализовать самостоятельно

1. Создай TelemetryService : QObject с running, packetsReceived и lastValue properties.

2. Добавь start()/stop() и внутренний QTimer; на timeout увеличивай счётчики и эмить корректные NOTIFY signals.

3. Раздели состояние и уведомление: property меняется только когда значение реально изменилось.

4. Сделай маленький C++ smoke test вызовов start/stop; QML пока может отображать только несколько properties.

Не распыляться сейчас

- Не создавай QThread.

- Не добавляй mutex: весь код сейчас намеренно живёт в одном потоке.

- Не используй shared_ptr для QObject просто потому, что это C++.

- Не делай сигналы «на всё подряд» — сигнал должен выражать событие/изменение состояния.

Что попросить Codex проверить именно в этой задаче

- QObject ownership: кто уничтожает QTimer и TelemetryService.

- Есть ли ложные/лишние NOTIFY emissions.

- Не хранится ли raw QObject\* с неясным lifetime.

- Не выполняется ли тяжёлая работа внутри timeout/slot (пока её вообще не должно быть).

Definition of Done

- [ ] Многократные start/stop не создают несколько активных таймеров.

- [ ] Properties и signals согласованы.

- [ ] Уничтожение сервиса не оставляет callbacks в мёртвый объект.

После задачи ты должен уметь объяснить без документации

- Чем QObject ownership отличается от обычного RAII?

- Что именно даёт Q_OBJECT?

- Почему slot — это не отдельный поток?

- Кто выбирает, когда вызовется slot: sender или event loop?

## 2. Qt Quick/QML и правильная C++↔QML граница

**Связанные экраны макета:** №1 Dashboard + общий shell/левое меню для будущих экранов.

**Результат задачи:** UI становится полноценным потребителем C++ API: Start/Stop, status, counters — без того, чтобы C++ искал QML-элементы и менял их напрямую.

Технологии, которые нужно понять перед кодом

- Qt Quick: Item/Window, anchors/layouts, Controls.

- Property bindings и signal handlers в QML.

- QML_ELEMENT (или другой явно выбранный registration mechanism).

- Экспорт QObject properties/methods/signals в QML.

Прочитать только это, затем сразу писать

- [Qt Quick](https://doc.qt.io/qt-6/qtquick-index.html) — базовые concepts и module usage

- [QML and C++ Integration Overview](https://doc.qt.io/qt-6/qtqml-cppintegration-overview.html) — сначала прочитай раздел выбора integration method

- [Defining QML Types from C++](https://doc.qt.io/qt-6/qtqml-cppintegration-definetypes.html) — QML_ELEMENT и registration

- [Best Practices for QML and Qt Quick](https://doc.qt.io/qt-6/qtquick-bestpractices.html) — особенно Exposing Data from C++ to QML

Что реализовать самостоятельно

1. Зарегистрируй TelemetryService в QML выбранным современным способом; не используй context property как основной архитектурный механизм.

2. Сделай общий `ApplicationWindow` с постоянным левым меню: Dashboard / Устройства / Журнал / Настройки. Для навигации используй `StackView` или простой эквивалент.

3. Сделай `DashboardPage.qml` по структуре экрана №1: четыре summary-карточки, placeholder области activity graph и блок последних событий. Пока реальные данные нужны только для status/counters — график и события станут живыми позже.

4. Подключи Start/Stop/status/counters через bindings.

5. Разбей UI минимум на 2–3 QML components, когда появится реальная повторяемость/смысл, а не заранее.

6. Проверь, что бизнес-состояние остаётся в C++; QML отвечает в основном за presentation/interaction.

Не распыляться сейчас

- Не вызывай QObject::findChild из C++ для управления UI.

- Не клади parsing/networking logic в JavaScript handlers QML.

- Не начинай кастомный rendering/QQuickItem.

- Не трать время на дизайн-систему, анимации и пиксель-перфект.

Что попросить Codex проверить именно в этой задаче

- Не тянет ли C++ dependency на QML object tree.

- Корректны ли Q_PROPERTY NOTIFY и bindings.

- Нет ли QObject ownership ambiguity между C++ и QML.

- Не используется ли context property там, где type registration проще и прозрачнее.

Definition of Done

- [ ] Shell содержит пункты Dashboard / Устройства / Журнал / Настройки.

- [ ] Dashboard визуально имеет структуру экрана №1, даже если часть блоков пока placeholder.

- [ ] UI полностью отражает доступное состояние TelemetryService через bindings.

- [ ] C++ ничего не знает о конкретных id/objectName в QML.

- [ ] Start/Stop работает через публичный C++ interface.

После задачи ты должен уметь объяснить без документации

- Почему лучше, чтобы QML зависел от C++ interface, а C++ не зависел от конкретного QML?

- Что произойдёт с binding, если property не имеет корректного NOTIFY?

- Кто владеет C++ object, созданным из QML?

## 3. QAbstractListModel: устройства как реальные данные

**Связанные экраны макета:** №2 Список устройств и №3 Детальная страница устройства.

**Результат задачи:** UI показывает динамический список устройств из C++ model; добавление и изменение устройств проходит через контракт Model/View.

Технологии, которые нужно понять перед кодом

- QAbstractListModel / QAbstractItemModel.

- rowCount(), data(), roleNames().

- beginInsertRows/endInsertRows и dataChanged.

- QML ListView/TableView + required role properties.

Прочитать только это, затем сразу писать

- [Models and Views in Qt Quick](https://doc.qt.io/qt-6/qtquick-modelviewsdata-modelview.html) — model/view/delegate и C++ data models

- [Using C++ Models with Qt Quick Views](https://doc.qt.io/qt-6/qtquick-modelviewsdata-cppmodels.html) — основной практический пример C++ model → QML

- [QSortFilterProxyModel](https://doc.qt.io/qt-6/qsortfilterproxymodel.html) — поиск/фильтрация/сортировка поверх исходной model

- [QAbstractListModel](https://doc.qt.io/qt-6/qabstractlistmodel.html) — минимальный контракт list model

- [Model/View Programming](https://doc.qt.io/qt-6/model-view-programming.html) — прочитай только model classes + изменение model data

Что реализовать самостоятельно

1. Создай value type DeviceState (id, status, temperature/metric, packet counter — без QObject, если не нужен).

2. Создай DeviceListModel : QAbstractListModel, который хранит collection DeviceState.

3. Добавь update-or-insert API: новое id создаёт row, существующее обновляет нужные поля и эмит dataChanged только для изменившихся roles.

4. Подключи model к TelemetryService и реализуй экран №2: поиск, фильтр группы, сортировку, таблицу/список и переход по строке. Для C++ варианта можешь использовать `QSortFilterProxyModel`; не копируй данные в отдельную QML-модель. Не усложняй pagination: если она мешает Model/View теме, сначала сделай виртуализированный список и добавь pagination позже.

5. Реализуй экран №3: header устройства + вкладки Обзор / Графики / Показатели / События / Настройки. На этом этапе полностью работает только Обзор с текущими properties; остальные вкладки могут быть честными placeholders.

6. Выбранное устройство передавай через ID/стабильный C++ API, а не через pointer на QML delegate.

7. Пока подавай данные тем же QTimer из задачи 1, чтобы isolating networking/threading не мешал освоить model contract.

Не распыляться сейчас

- Не делай один QObject на каждую строку без причины.

- Не вызывай beginResetModel для каждого маленького изменения.

- Не меняй внутренний vector в обход begin/end notifications.

- Не трогай модель из другого потока — пока все updates идут GUI thread.

Что попросить Codex проверить именно в этой задаче

- Model/View invariants и корректные QModelIndex checks.

- Правильно ли ограждены beginInsertRows/endInsertRows.

- Минимальны ли dataChanged roles/ranges.

- Нет ли случайной зависимости model от QML.

Definition of Done

- [ ] Экран №2 имеет поиск/фильтр/сортировку и переход к выбранному устройству.

- [ ] Экран №3 показывает Overview и не хранит бизнес-данные только в QML.

- [ ] Добавление нового device немедленно появляется в view.

- [ ] Обновление существующего device не пересоздаёт весь model.

- [ ] 100+ fake devices обновляются без заметного зависания UI.

После задачи ты должен уметь объяснить без документации

- Зачем beginInsertRows до изменения контейнера и endInsertRows после?

- Почему model обычно должен жить в GUI thread?

- Когда dataChanged лучше resetModel?

- Что такое roleNames и как QML получает роли?

## 4. Qt Test как часть цикла, а не финальная уборка

**Связанные экраны макета:** Инфраструктура тестов для backend/models; UI пока не расширяется.

**Результат задачи:** после этой задачи каждый следующий milestone имеет автоматическую проверку; model contract тоже проверяется инструментом Qt.

Технологии, которые нужно понять перед кодом

- Qt Test / QTest.

- QSignalSpy для сигналов.

- QAbstractItemModelTester для инвариантов модели.

- QTRY_VERIFY/QTRY_COMPARE для event-driven asynchronous checks.

Прочитать только это, затем сразу писать

- [Qt Test Overview](https://doc.qt.io/qt-6/qtest-overview.html) — structure of tests и execution

- [QSignalSpy](https://doc.qt.io/qt-6/qsignalspy.html) — проверка сигналов

- [QAbstractItemModelTester](https://doc.qt.io/qt-6/qabstractitemmodeltester.html) — автоматическая проверка model invariants

Что реализовать самостоятельно

1. Добавь отдельные test executables для TelemetryService и DeviceListModel или один небольшой test target с логическими suites.

2. Проверь start/stop/idempotency TelemetryService и количество/значения ключевых signals через QSignalSpy.

3. Подключи QAbstractItemModelTester к DeviceListModel в unit tests.

4. Добавь сценарии insert/update нескольких devices и проверку rows/data.

5. Убедись, что ctest запускает тесты без ручного открытия GUI.

Не распыляться сейчас

- Не мокай Qt internals.

- Не тестируй визуальный цвет/координаты QML на этом этапе.

- Не используй sleep как основной механизм ожидания async события; используй QTRY\_\* / signal-driven verification.

Что попросить Codex проверить именно в этой задаче

- Не являются ли тесты timing-dependent/flaky.

- Проверяются ли failure paths, а не только happy path.

- Есть ли tests, которые реально поймают неверные model notifications.

- Не проходит ли тест случайно из-за shared global state.

Definition of Done

- [ ] ctest проходит после clean build.

- [ ] QAbstractItemModelTester подключён и не выдаёт warnings.

- [ ] Намеренно сломанный NOTIFY/model update приводит к падению релевантного теста.

После задачи ты должен уметь объяснить без документации

- Почему QSignalSpy лучше ручного bool-флага?

- Почему sleep в async тесте делает тест слабее?

- Что именно проверяет QAbstractItemModelTester?

## 5. Отдельный simulator process + UDP networking

**Связанные экраны макета:** №7 Добавление источника: в core реально работает UDP-конфигурация.

**Результат задачи:** данные впервые приходят извне: simulator шлёт telemetry datagrams, app получает их асинхронно, валидирует и обновляет model. Всё ещё без QThread.

Технологии, которые нужно понять перед кодом

- QCoreApplication для headless simulator.

- QUdpSocket и readyRead-driven I/O.

- QJsonDocument/QJsonObject как простой первый protocol.

- QHostAddress и базовая error handling.

- Event loop: почему asynchronous socket не требует отдельного blocking read loop.

Прочитать только это, затем сразу писать

- [Qt Network Programming](https://doc.qt.io/qt-6/qtnetwork-programming.html) — overview: sockets are asynchronous/event-driven

- [QUdpSocket](https://doc.qt.io/qt-6/qudpsocket.html) — bind, readyRead, receiveDatagram/writeDatagram

- [QNetworkDatagram](https://doc.qt.io/qt-6/qnetworkdatagram.html) — datagram data/sender metadata

- [QJsonDocument](https://doc.qt.io/qt-6/qjsondocument.html) — parse/serialize JSON

Что реализовать самостоятельно

1. Введи value type `SourceConfig` минимум с `type`, `address`, `port`, `name`, `autoConnect`.

2. Реализуй экран №7 `AddSourceDialog`: type/address/port/name/auto-connect + валидация. В core-плане dropdown содержит минимум `UDP Socket`; `TCP Socket` можно показать как planned/disabled option до post-core расширения.

3. После Add конфигурация передаётся C++ source manager/service; не создавай socket прямо из QML handler.

4. В simulator создай QTimer, который генерирует несколько device IDs и отправляет UDP datagrams на localhost.

5. Определи маленький versioned JSON schema, например protocolVersion/deviceId/sequence/timestamp/value. Не добавляй лишние поля.

6. В app добавь UdpReceiver в GUI thread: bind socket, реагируй на readyRead, прочитай все pending datagrams.

7. Отдели decode/validation от socket code обычной C++ функцией, которую можно unit-test без сети.

8. На валидный sample вызывай update-or-insert model; на malformed packet — счётчик ошибок/лог, но приложение не падает.

Не распыляться сейчас

- Не используй waitForReadyRead в GUI thread.

- Не создавай thread на каждый socket/device.

- Не делай TCP/reconnect ещё — сначала event-driven UDP path.

- Не усложняй протокол protobuf/CBOR/custom binary пока JSON не доказал узкое место.

Что попросить Codex проверить именно в этой задаче

- Не блокирует ли socket path GUI event loop.

- Корректно ли вычитываются все pending datagrams.

- Обрабатываются ли malformed/oversized/missing fields безопасно.

- Разделены ли transport и decoding достаточно для unit tests.

Definition of Done

- [ ] Экран №7 создаёт валидную UDP source configuration через C++ API.

- [ ] Simulator и app запускаются независимо.

- [ ] Несколько devices обновляются через реальный UDP.

- [ ] Malformed packet не ломает app и покрыт тестом decoder.

- [ ] Нет blocking socket calls в GUI thread.

После задачи ты должен уметь объяснить без документации

- Почему QUdpSocket может работать без отдельного thread?

- Что делает event loop после readyRead?

- Почему parsing лучше отделить от socket object?

- Какие гарантии UDP не даёт и почему sequence number полезен?

## 6. QThread + worker-object pattern + thread affinity

**Связанные экраны макета:** №1–3 продолжают получать те же данные, но transport переезжает в I/O thread.

**Результат задачи:** network I/O переезжает в отдельный I/O thread, а model остаётся в GUI thread. Ты видишь на практике queued delivery и lifetime правил QObject.

Технологии, которые нужно понять перед кодом

- QThread как объект, управляющий OS thread; QThread object сам живёт не «внутри себя».

- QObject::moveToThread() и thread affinity.

- Отдельный event loop в worker thread.

- Qt::AutoConnection / DirectConnection / QueuedConnection.

- deleteLater и корректное создание child objects в нужном thread.

Прочитать только это, затем сразу писать

- [Threads and QObjects](https://doc.qt.io/qt-6/threads-qobject.html) — самая важная статья задачи

- [QThread](https://doc.qt.io/qt-6/qthread.html) — worker-object approach и lifecycle

- [QObject thread affinity](https://doc.qt.io/qt-6/qobject.html) — Thread Affinity section

- [Qt::ConnectionType](https://doc.qt.io/qt-6/qt.html) — Direct/Queued/BlockingQueued semantics

Что реализовать самостоятельно

1. Преобразуй UdpReceiver в UdpReceiverWorker : QObject, но не меняй public semantics приложения.

2. Создай QThread в owner/service на GUI thread; worker создай без parent и moveToThread(thread).

3. Создавай QUdpSocket внутри worker start()/initialize slot, который реально выполняется уже в worker thread. Не создавай socket заранее в GUI thread и потом «надеясь» move.

4. Передавай decoded sample/batch обратно через signal. Получатель, который меняет DeviceListModel, должен жить в GUI thread.

5. В debug/test добавь проверки QThread::currentThread()/QObject::thread(), чтобы увидеть фактическую affinity.

6. Сделай start/stop API так, чтобы GUI никогда не вызывал методы worker напрямую из неправильного потока.

Не распыляться сейчас

- Не вызывай moveToThread(this) внутри subclass QThread.

- Не используй QThread::terminate().

- Не трогай DeviceListModel из worker.

- Не добавляй mutex «на всякий случай» вместо правильной передачи сообщений.

- Не используй BlockingQueuedConnection без очень конкретной причины.

Что попросить Codex проверить именно в этой задаче

- Где создан каждый QObject и где он живёт после moveToThread.

- Создаётся ли QUdpSocket в thread с рабочим event loop.

- Каким connection type реально будет доставлен каждый cross-thread signal.

- Нет ли прямого вызова метода worker из GUI thread.

- Корректен ли lifetime worker/thread и deleteLater sequence.

Definition of Done

- [ ] Лог показывает network callbacks в I/O thread, model updates в GUI thread.

- [ ] UI остаётся отзывчивым при большой частоте datagrams.

- [ ] Thread sanitizer/обычный стресс не выявляет очевидных races (если sanitizer доступен).

- [ ] Start/Stop проходит многократно без crash/warning QObject::... from wrong thread.

После задачи ты должен уметь объяснить без документации

- В каком потоке живёт сам объект QThread?

- Где выполнится slot при AutoConnection между объектами разных threads?

- Почему socket лучше создать внутри worker после moveToThread?

- Что такое thread affinity и чем она отличается от mutex/thread-safety?

## 7. Graceful shutdown и lifecycle под нагрузкой

**Связанные экраны макета:** №9 Ошибка подключения + корректное состояние источника/сервиса.

**Результат задачи:** поток, worker и socket гарантированно останавливаются без terminate, use-after-free и «QThread: Destroyed while thread is still running».

Технологии, которые нужно понять перед кодом

- QObject::deleteLater() и deferred deletion.

- QThread::quit(), finished(), wait().

- Состояния сервиса (Stopped/Starting/Running/Stopping/Error) через enum + Q_ENUM.

- Идемпотентный start/stop.

Прочитать только это, затем сразу писать

- [QThread](https://doc.qt.io/qt-6/qthread.html) — особенно destructor/quit/wait/finished

- [QObject](https://doc.qt.io/qt-6/qobject.html) — deleteLater, destroyed, parent ownership

- [Threads and QObjects](https://doc.qt.io/qt-6/threads-qobject.html) — deletion and event-loop constraints

Что реализовать самостоятельно

1. Введи явное состояние TelemetryService; UI не должен угадывать состояние по наличию pointer/thread.

2. Определи единственный shutdown sequence: прекратить новые события → закрыть socket/worker → quit thread → wait → освободить owner-side ресурсы.

3. Проверь сценарий закрытия приложения во время активной telemetry.

4. Проверь 20+ последовательных start/stop циклов в integration test или ручном стресс-режиме.

5. Добавь обработку ошибок bind/socket и переход в Error без утечки потока.

6. Прокинь человекочитаемую ошибку в QML и реализуй экран №9: заголовок, source/address, короткая рекомендация и OK.

7. Одновременно создавай пользовательское событие об ошибке через временный service API; полноценный EventListModel появится в задаче 11.

Не распыляться сейчас

- Не «лечи» shutdown через sleep.

- Не удаляй worker напрямую из GUI thread, если он ещё живёт в worker thread.

- Не вызывай terminate для удобства.

- Не усложняй QStateMachine — простой enum сейчас достаточен.

Что попросить Codex проверить именно в этой задаче

- Порядок остановки и возможные гонки сигналов после stop.

- Может ли callback попасть в уже уничтоженный owner/model.

- Может ли wait зависнуть из-за event loop/worker state.

- Есть ли path, где thread destructor вызывается while running.

Definition of Done

- [ ] Закрытие приложения во время load завершается чисто.

- [ ] Многократный start/stop не накапливает threads/sockets/connections.

- [ ] Bind failure приводит к понятному Error state и открывает экран №9.

- [ ] После закрытия ошибки можно исправить config и запустить источник повторно без restart app.

- [ ] Нет terminate и нет произвольных sleep в production code.

После задачи ты должен уметь объяснить без документации

- Почему deleteLater зависит от event loop?

- Почему QThread::quit не «убивает» поток мгновенно?

- Какой объект должен отвечать за join/wait?

- Что произойдёт, если уничтожить QThread пока управляемый thread ещё работает?

## 8. Batching и давление на event queue

**Связанные экраны макета:** №1 Dashboard: живой activity graph/counters и ограниченная частота UI updates.

**Результат задачи:** вместо signal/model update на каждый пакет данные проходят пакетами; GUI получает ограниченное число обновлений и остаётся отзывчивым при высокой частоте telemetry.

Технологии, которые нужно понять перед кодом

- Queued signals копируют/переносят аргументы через event queue.

- Batching как способ уменьшить signal/slot + model-update overhead.

- QTimer в worker thread как flush timer.

- QVector/value types; Q_DECLARE_METATYPE/qRegisterMetaType только если реально требуется типу queued delivery.

- Qt Quick performance: models/views и worker batching.

Прочитать только это, затем сразу писать

- [Qt Quick Performance](https://doc.qt.io/qt-6/qtquick-performance.html) — прочитай Models and Views + asynchronous/batching recommendations

- [Queued connection semantics](https://doc.qt.io/qt-6/qt.html) — ConnectionType и требования к аргументам

- [Creating Custom Qt Types](https://doc.qt.io/qt-6/custom-types.html) — metatype registration только если твой type этого требует

Что реализовать самостоятельно

1. Замерь baseline: сколько model updates/signals в секунду при текущей реализации.

2. В worker накопи samples в небольшой buffer и flush по одному из условий: размер batch или короткий timer interval. Порог выбери экспериментом, а не «магическим» копированием.

3. Передавай один immutable batch в GUI thread; DeviceListModel применяет изменения группой, сохраняя корректные notifications.

4. Добавь counters: received packets, emitted batches, model update batches, dropped/invalid packets.

5. Добавь bounded activity history для экрана №1: например samples receive/update rate за последнюю минуту. History обязана иметь явную верхнюю границу.

6. Подключи Dashboard summary cards и activity series к реальным данным.

7. Сравни responsiveness и event frequency до/после.

Не распыляться сейчас

- Не держи mutex вокруг GUI model.

- Не передавай pointer на worker-owned mutable buffer.

- Не делай гигантские batches, которые сами блокируют GUI надолго.

- Не оптимизируй JSON/parser пока не измерил, что именно bottleneck.

Что попросить Codex проверить именно в этой задаче

- Lifetime/copy semantics batch data через queued signal.

- Не передаётся ли reference/pointer на память, которую worker изменит раньше GUI.

- Корректны ли model notifications при batch apply.

- Не создаёт ли batching скрытую latency/потерю данных на shutdown.

Definition of Done

- [ ] Число cross-thread GUI updates заметно меньше числа datagrams.

- [ ] Model получает только value/immutable data.

- [ ] На stop остаток buffer либо flush-ится по выбранной политике, либо явно отбрасывается и это документировано.

- [ ] Dashboard №1 показывает реальные total/online/offline/error counters и живую bounded activity history.

- [ ] UI остаётся отзывчивым при усиленном simulator load.

После задачи ты должен уметь объяснить без документации

- Почему queued signal на каждый packet может стать проблемой?

- Что именно копируется/хранится до выполнения queued slot?

- Почему batching снижает overhead, но увеличивает latency?

- Как выбрать размер/интервал batch инженерно?

## 9. QThreadPool для CPU-bound работы + backpressure

**Связанные экраны макета:** №3 вкладка показателей и №4 Графики: history, min/max/avg, Qt Graphs.

**Результат задачи:** тяжёлая обработка telemetry отделяется от I/O event loop и GUI; количество одновременно/ожидающе выполняемой работы ограничено.

Технологии, которые нужно понять перед кодом

- QThreadPool и callable/QRunnable jobs.

- Разница: long-lived event-loop worker vs short CPU task.

- QSemaphore как простой лимит outstanding jobs.

- QMetaObject::invokeMethod(..., Qt::QueuedConnection) или сигнал через стабильный QObject для возврата результата в owner thread.

- Bounded history для выбранного устройства.

- Qt Graphs 2D: `GraphsView`, `LineSeries`, axes.

- Безопасные lambda captures и value semantics.

Прочитать только это, затем сразу писать

- [QThreadPool](https://doc.qt.io/qt-6/qthreadpool.html) — pool behavior, globalInstance, start/tryStart

- [QRunnable](https://doc.qt.io/qt-6/qrunnable.html) — ownership/autoDelete, даже если используешь callable

- [QSemaphore](https://doc.qt.io/qt-6/qsemaphore.html) — ограничение количества одновременно допущенных jobs

- [QMetaObject](https://doc.qt.io/qt-6/qmetaobject.html) — invokeMethod и queued invocation

- [Qt Graphs](https://doc.qt.io/qt-6/qtgraphs-index.html) — только 2D overview

- [Qt Graphs 2D QML](https://doc.qt.io/qt-6/graphs-qml-2d.html) — `GraphsView`, `LineSeries`, axes

Что реализовать самостоятельно

1. Выбери реальную CPU-задачу: например rolling statistics/aggregation по batch, percentile/min/max или более тяжёлая validation/transform.

2. Сначала реализуй её синхронно как pure/value-oriented function и unit-test результат.

3. Перенеси вызов функции в QThreadPool. Никаких QObject/model accesses внутри job.

4. Верни immutable result в нужный QObject thread через queued delivery.

5. Добавь backpressure: ограничь число outstanding jobs через QSemaphore/tryAcquire или собственный очень маленький bounded policy; при перегрузке измеримо drop/coalesce работу вместо бесконечного роста queue.

6. Добавь metrics: pool active count, outstanding jobs, dropped/coalesced work, processing latency.

7. Добавь bounded history по каждому устройству/показателю. Не храни бесконечный поток.

8. Реализуй экран №4 через Qt Graphs: период, metric selector, line chart, min/max/avg. Расчёт более тяжёлой статистики должен использовать результат CPU pipeline, а не блокировать QML handler.

9. Подключи вкладку Graphs экрана №3 к экрану №4.

Не распыляться сейчас

- Не создавай одну job на каждый крошечный sample — работай с batch.

- Не захватывай raw this в lambda, если task может пережить owner.

- Не вызывай UI/model из thread-pool thread.

- Не увеличивай maxThreadCount бесконечно «чтобы быстрее».

Что попросить Codex проверить именно в этой задаче

- Lambda captures и lifetime: может ли owner умереть до завершения task.

- Где release-ится semaphore при всех exit/error paths.

- Нет ли unbounded task submission.

- Не держится ли lock во время тяжёлого CPU compute.

- Результат возвращается в правильный thread и не обращается к уничтоженному object.

Definition of Done

- [ ] CPU work реально выполняется в pool thread (проверено логом/thread id).

- [ ] I/O worker продолжает читать socket и не блокируется CPU-вычислением.

- [ ] Outstanding work ограничен; memory не растёт без границ при overload.

- [ ] Экран №4 показывает реальную bounded history, выбранный metric и min/max/avg.

- [ ] После stop новые jobs не создаются, оставшиеся безопасно завершаются/игнорируются по явной политике.

После задачи ты должен уметь объяснить без документации

- Когда QThreadPool лучше QThread?

- Почему thread pool не делает твою функцию thread-safe автоматически?

- Что случится, если submit rate стабильно выше processing rate?

- Почему backpressure — часть correctness, а не только optimization?

## 10. QtConcurrent + QFuture/QPromise: отдельный job с progress/cancel

**Связанные экраны макета:** №8 Экспорт данных и кнопка Export на №4.

**Результат задачи:** в проекте появляется вторая модель concurrency: пользователь запускает export/analysis большого history snapshot, видит progress и может отменить операцию без блокировки GUI.

Технологии, которые нужно понять перед кодом

- QtConcurrent::run().

- QFuture и QFutureWatcher.

- QPromise для progress/result/cancellation в promise mode.

- Snapshot/copy data before background job вместо чтения QAbstractItemModel из worker.

Прочитать только это, затем сразу писать

- [Qt Concurrent](https://doc.qt.io/qt-6/qtconcurrent-index.html) — overview и выбор high-level API

- [Concurrent Run](https://doc.qt.io/qt-6/qtconcurrentrun.html) — basic mode + Run With Promise

- [QFuture](https://doc.qt.io/qt-6/qfuture.html) — result/status/cancel semantics

- [QFutureWatcher](https://doc.qt.io/qt-6/qfuturewatcher.html) — signals для GUI

- [QPromise](https://doc.qt.io/qt-6/qpromise.html) — progress/cancellation cooperation

- [Qt Quick Dialogs](https://doc.qt.io/qt-6/qtquickdialogs-index.html) — `FileDialog` для выбора пути

Что реализовать самостоятельно

1. Добавь bounded history в owner/GUI-side data store (например последние N batches/samples) без фонового доступа к model internals.

2. Реализуй экран №8: От/До, format, device selection, file path, Export/Cancel. Для file picker используй Qt Quick Dialogs `FileDialog`; core-format — CSV.

3. На команду Export/Analyze сначала создай immutable snapshot необходимых данных.

4. Запусти job через QtConcurrent::run; в job периодически проверяй cancellation и сообщай progress через QPromise.

5. В GUI thread подключи QFutureWatcher к progress/finished/canceled и покажи минимальный ProgressBar/Cancel button.

6. Сделай реальную работу: CSV export в выбранный file path или вычисление сводной статистики. Не симулируй sleep.

Не распыляться сейчас

- Не вызывай future.result() в GUI thread до finished — это может блокировать.

- Не передавай QAbstractItemModel в background job.

- Не делай cancellation «магической»: worker function должна кооперативно её проверять.

- Не используй QtConcurrent там, где нужен долгоживущий event loop/socket worker.

Что попросить Codex проверить именно в этой задаче

- Блокируется ли где-нибудь GUI ожиданием future.

- Snapshot действительно независим от изменяемой model/store памяти.

- Все cancellation/error exits приводят UI state в норму.

- QFutureWatcher lifetime не короче future/task callbacks.

Definition of Done

- [ ] Экран №8 принимает реальный диапазон/устройства/путь и показывает progress/cancel.

- [ ] Export/analysis не блокирует UI.

- [ ] Progress движется из реального выполнения, Cancel реально прекращает дальнейшую работу.

- [ ] Повторный запуск после cancel/finish работает.

- [ ] В README есть 3 строки: почему эта задача — QtConcurrent, а network worker — QThread.

После задачи ты должен уметь объяснить без документации

- Чем QFuture отличается от std::future в контексте Qt event-driven UI?

- Почему QFutureWatcher удобен GUI?

- Почему cancellation обычно cooperative?

- Почему snapshot часто проще и безопаснее shared mutable store + locks?

## 11. Logging, metrics и наблюдаемость

**Связанные экраны макета:** №5 Журнал событий + последние события на №1.

**Результат задачи:** появляются две разные системы наблюдаемости: diagnostic logging для разработчика и пользовательский журнал событий (экран №5).

Технологии, которые нужно понять перед кодом

- QLoggingCategory и qCDebug/qCInfo/qCWarning.

- QT_LOGGING_RULES для включения категорий без recompilation.

- QElapsedTimer для latency/throughput measurements.

- `EventListModel : QAbstractListModel` для пользовательских событий и фильтрации в UI.

- Чёткие counters вместо ощущения «вроде быстро».

Прочитать только это, затем сразу писать

- [QLoggingCategory](https://doc.qt.io/qt-6/qloggingcategory.html) — categories и runtime filtering

- [QElapsedTimer](https://doc.qt.io/qt-6/qelapsedtimer.html) — monotonic elapsed measurements

- [Debugging Techniques](https://doc.qt.io/qt-6/debug.html) — Qt logging/debug basics

Что реализовать самостоятельно

1. Создай value type `UserEvent` и `EventListModel` с timestamp/device/severity/message.

2. Реализуй экран №5: search, level filter, device filter, clear и список событий.

3. Подключи последние несколько `UserEvent` к Dashboard №1.

4. Создай категории telemetry.network, telemetry.pipeline, telemetry.model, telemetry.export.

5. Замени диагностические qDebug на category-based logging; ошибки protocol/socket должны содержать достаточный context без dumping огромных payloads.

6. Добавь measurements: packet receive rate, batch rate/size, processing latency, outstanding jobs, drop count, model apply latency.

7. Сделай один компактный diagnostics block в UI или периодический summary log.

8. Проверь, что verbose categories можно выключить через QT_LOGGING_RULES.

Не распыляться сейчас

- Не логируй каждый packet на high load как обязательный production path.

- Не смешивай metrics и UI business state без нужды.

- Не оптимизируй по одному случайному замеру.

Что попросить Codex проверить именно в этой задаче

- Нет ли логов внутри hot path, которые сами создают bottleneck.

- Все ошибки содержат полезный context, но не зависят от GUI.

- Метрики обновляются thread-safe способом: либо owner-thread updates, либо безопасные atomics/value handoff.

- Measurements используют monotonic elapsed timer, где это важно.

Definition of Done

- [ ] Экран №5 показывает пользовательские события и не является прямым отображением qDebug.

- [ ] Dashboard показывает последние события.

- [ ] Можно одной env-настройкой включить подробные network/pipeline logs.

- [ ] Нагрузочный запуск выдаёт измеримый summary, а не субъективное впечатление.

- [ ] Логи не меняют correctness/lifetime и не засоряют UI thread.

После задачи ты должен уметь объяснить без документации

- Зачем QLoggingCategory, если есть qDebug?

- Почему wall clock хуже QElapsedTimer для duration?

- Какие 3 метрики первыми покажут overload pipeline?

## 12. Настройки и QSettings

**Связанные экраны макета:** №6 Настройки.

**Результат задачи:** пользователь меняет основные preferences, нажимает Save, перезапускает приложение и получает те же значения.

### Технологии, которые нужно понять перед кодом

- `QSettings` как platform-independent persistence для preferences.
- `Q_PROPERTY`/signals для SettingsService.
- Qt Quick Controls: `ComboBox`, `Switch`, `SpinBox`/`TextField`, `Button`.
- Разница между business state и user preferences.

### Прочитать только это, затем сразу писать

- [QSettings](https://doc.qt.io/qt-6/qsettings.html) — basic usage, keys/groups, defaults, sync/status.
- [Qt Quick Controls](https://doc.qt.io/qt-6/qtquick-controls-qmlmodule.html) — только controls, которые реально понадобятся форме.
- [QCoreApplication](https://doc.qt.io/qt-6/qcoreapplication.html) — application/organization name для стабильного settings storage.

### Что реализовать самостоятельно

1. Создай `SettingsService : QObject`, который централизованно читает/пишет `QSettings`.
2. Properties минимум: `theme`, `language`, `visualRefreshMs`, `measurementUnits`, `defaultExportPath`, `defaultExportFormat`.
3. Реализуй экран №6 и его левое меню разделов. Полностью рабочим сделай раздел «Общие». Раздел «Подключения» должен показывать сохранённые `SourceConfig` и открывать экран №7; сохрани минимум type/address/port/name/autoConnect. Остальные разделы могут быть минимальными, но должны иметь понятные места назначения из ТЗ.
4. Кнопка Save валидирует значения и сохраняет их. После restart значения восстанавливаются.
5. Theme реально переключает хотя бы dark/light presentation. System theme можно отложить, если platform API начинает отвлекать от Qt Core/QML темы.
6. «Автозапуск» и «минимизация в трей» храни как preferences, но реальную OS integration не делай в core-плане; явно пометь это в UI/README как post-core.
7. Default export path/format используется экраном №8, а не хранится отдельно второй копией.

### Не распыляться сейчас

- Не делай собственный config parser вместо `QSettings`.
- Не размазывай `QSettings` calls по десяткам QML/C++ файлов.
- Не начинай platform-specific registry/login-items/systemd/autostart интеграцию.
- Не делай огромную theme engine.

### Что попросить Codex проверить

- Есть ли один owner для settings API.
- Корректны ли defaults и обработка corrupted/invalid values.
- Не дублируется ли state между SettingsService и QML controls.
- Не пишет ли код settings в hot path без причины.

### Definition of Done

- [ ] Экран №6 существует и соответствует ТЗ.
- [ ] Основные настройки переживают restart.
- [ ] Dark/light тема реально меняется.
- [ ] Settings → Connections показывает сохранённые источники и открывает Add Source.
- [ ] Export dialog получает defaults из SettingsService.
- [ ] Invalid refresh interval не сохраняется как рабочее значение.

### После задачи ты должен уметь объяснить

- Где физически хранится `QSettings` на разных ОС на концептуальном уровне?
- Почему лучше скрыть `QSettings` за SettingsService?
- Что считать default value и где оно должно быть определено?

## 13. Load testing и Qt Quick performance

**Связанные экраны макета:** №1, №2 и №4 под нагрузкой.

**Результат задачи:** ты не просто говоришь «многопоточно», а показываешь, что GUI не захлёбывается и pipeline имеет контролируемое поведение под нагрузкой.

Технологии, которые нужно понять перед кодом

- Qt Quick performance guidelines.

- QML Profiler / Qt Creator analyzer (если доступен в твоём setup).

- Release build vs Debug.

- Throughput/latency/backpressure trade-offs.

Прочитать только это, затем сразу писать

- [Qt Quick Performance](https://doc.qt.io/qt-6/qtquick-performance.html) — обязательно ещё раз Models/Views/worker recommendations

- [Qt Creator QML Profiler](https://doc.qt.io/qtcreator/creator-qml-performance-monitor.html) — посмотри workflow, если используешь Qt Creator

- [QThreadPool](https://doc.qt.io/qt-6/qthreadpool.html) — activeThreadCount/maxThreadCount для интерпретации metrics

Что реализовать самостоятельно

1. Добавь simulator presets хотя бы low / medium / overload: меняй device count и datagrams rate.

2. Запусти Release build и сними baseline metrics: receive rate, batch rate, processing latency, drop/coalesce count, GUI update rate.

3. Найди один реальный bottleneck с помощью measurements/profiler и исправь его. Не делай список «оптимизаций по памяти» без измерений.

4. Отдельно проверь Dashboard №1, список №2 и график №4: переключения страниц/metric selector и обновления chart не должны превращать QML binding loop в bottleneck.

5. Сохрани таблицу before/after в README.

6. Проверь memory behaviour при длительном overload: queue/history обязаны иметь явные bounds.

Не распыляться сейчас

- Не придумывай целевой «60 FPS» если твой UI это не измеряет.

- Не сравнивай Debug и Release как будто это одна конфигурация.

- Не переписывай архитектуру ради микросекунд до измерения bottleneck.

Что попросить Codex проверить именно в этой задаче

- Правильна ли интерпретация measurements и не измеряется ли сама logging overhead.

- Нет ли unbounded containers/queues/history.

- Batch/model updates не делают O(N²) работу по мере роста устройств.

- Optimization не нарушила thread affinity/model contract.

Definition of Done

- [ ] README содержит воспроизводимый load command/config и before/after metrics.

- [ ] Overload приводит к контролируемому drop/coalesce, а не unlimited memory growth.

- [ ] UI можно использовать во время high load.

- [ ] Ты можешь назвать найденный bottleneck и доказать, почему исправление помогло.

После задачи ты должен уметь объяснить без документации

- Чем throughput отличается от latency?

- Почему максимальный throughput без backpressure может ухудшить систему?

- Что ты будешь профилировать первым, если UI начинает лагать?

## 14. Concurrency tests и failure scenarios

**Связанные экраны макета:** Проверка failure paths экранов №7, №8, №9 и всего concurrency pipeline.

**Результат задачи:** проект проверяет не только happy path: shutdown во время load, malformed data, overload, cancel и повторный start.

Технологии, которые нужно понять перед кодом

- QSignalSpy/QTRY\_\* для asynchronous integration tests.

- Deterministic seams: decoder pure function, injectable port/config, ограниченные timeouts.

- Model tester на всех code paths, которые меняют модель.

Прочитать только это, затем сразу писать

- [QSignalSpy](https://doc.qt.io/qt-6/qsignalspy.html) — повторно для integration tests

- [QTest Namespace](https://doc.qt.io/qt-6/qtest.html) — QTRY\_\* macros и waits

- [QAbstractItemModelTester](https://doc.qt.io/qt-6/qabstractitemmodeltester.html) — оставь подключённым в model tests

Что реализовать самостоятельно

1. Тест: start → receive real localhost UDP → model has device → stop.

2. Тест: malformed packets не меняют model, но увеличивают error metric.

3. Тест: stop/close while simulator floods packets — test завершается, thread не остаётся running.

4. Тест: export cancel → finished/canceled state consistent → повторный export работает.

5. Тест: overload/backpressure — outstanding work не превышает установленный bound.

6. Тест: серия start/stop циклов без QObject/thread warnings.

7. Тест/сценарий: invalid source config не запускает network path; bind/connect error порождает Error state + пользовательское событие.

8. Тест/сценарий: закрытие Export dialog после cancel не оставляет watcher/job callbacks, обращающихся к уничтоженному UI state.

Не распыляться сейчас

- Не увеличивай timeout, чтобы «починить» race/flaky test.

- Не делай tests зависимыми от внешней сети/интернета.

- Не проверяй private fields напрямую, если observable contract достаточен.

Что попросить Codex проверить именно в этой задаче

- Flakiness/timing assumptions.

- Есть ли test, который воспроизводит каждый важный lifetime failure path.

- Assertions проверяют observable outcome, а не только «не упало».

- Cleanup test objects гарантирован после failure assertion.

Definition of Done

- [ ] ctest стабильно проходит несколько последовательных запусков.

- [ ] Нет случайных fixed sleeps как основы синхронизации.

- [ ] Хотя бы один тест гарантированно падает, если убрать backpressure/shutdown guard/model notification.

После задачи ты должен уметь объяснить без документации

- Как тестировать event-driven code без длинных sleep?

- Какой самый опасный race в твоём shutdown path?

- Как доказать тестом, что background task не пишет в model напрямую?

## 15. Финальная упаковка: README и интервью-аргументация

**Связанные экраны макета:** Все экраны №1–9, screenshots и финальная документация.

**Результат задачи:** репозиторий объясняет архитектуру лучше, чем список технологий в резюме; ты можешь защитить решения на собеседовании.

Технологии, которые нужно понять перед кодом

- Архитектурное описание data flow и thread ownership.

- Design rationale: почему разные concurrency primitives.

- Reproducible build/test/load commands.

Прочитать только это, затем сразу писать

- [Qt Threading Technologies overview](https://doc.qt.io/qt-6/threads-technologies.html) — сверь своё объяснение выбора инструментов

- [Threads and QObjects](https://doc.qt.io/qt-6/threads-qobject.html) — финальная проверка терминологии

- [QML/C++ Best Practices](https://doc.qt.io/qt-6/qtquick-bestpractices.html) — сверь UI/backend boundary

Что реализовать самостоятельно

1. README: коротко цель, **скриншоты всех экранов №1–9 с теми же номерами, что в `ui-reference.png`**, build/run, simulator run, tests, load preset.

2. Добавь одну архитектурную диаграмму с подписью thread affinity каждого QObject.

3. Опиши 3 design decisions: QThread для network event loop; QThreadPool для bounded CPU jobs; QtConcurrent/QFuture для user-triggered export/analysis.

4. Добавь таблицу load results и найденный bottleneck before/after.

5. Запиши раздел Known trade-offs / What I would change in production — 3–5 реальных пунктов, а не самоедство.

6. Сделай финальный Codex review всего concurrency/lifetime path, затем сам проверь каждое finding.

Не распыляться сейчас

- Не скрывай trade-offs фразой «всё production-ready».

- Не заполняй README большим туториалом по Qt.

- Не добавляй технологии только ради keywords в CV.

Что попросить Codex проверить именно в этой задаче

- End-to-end ownership graph: кто кого создаёт/останавливает/удаляет.

- Cross-thread call graph и все места, где execution context меняется.

- Unbounded queues/containers/tasks.

- Model/View + QML boundary.

- Missing failure tests и документация assumptions.

Definition of Done

- [ ] Новый человек может собрать и запустить проект по README.

- [ ] В README есть нумерованные screenshots экранов №1–9 и они соответствуют `02_PROJECT_SPEC.md`.

- [ ] Диаграмма показывает GUI/I/O/pool/concurrent jobs без двусмысленности.

- [ ] Ты можешь за 5 минут объяснить data flow и shutdown.

- [ ] В README есть измерения и тест-команды.

- [ ] Финальный clean build + ctest проходит.

После задачи ты должен уметь объяснить без документации

- Почему network worker — QThread, а не QtConcurrent?

- Почему CPU aggregation — QThreadPool, а не один QThread?

- Почему export — QtConcurrent/QFuture?

- Где именно происходит cross-thread handoff?

- Какие данные shared mutable, а какие передаются value-copy?

- Как система ведёт себя, если producer быстрее consumer?


# После core-плана

Не смешивай это с задачами 0–15. Только когда core закончен и протестирован:

1. Доведи экран №7 до полноценного `TCP Socket` через `QTcpSocket`.
2. Добавь reconnect/backoff state machine.
3. При желании реализуй системный tray/autostart/desktop notifications.
4. Если нужна persistent history — добавь Qt SQL/SQLite.

Смотри `03_ARCHITECTURE.md`, раздел «Опциональные расширения после core-плана».
