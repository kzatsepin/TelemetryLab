# Инструкция по прохождению проекта

## Цель

Написать проект самостоятельно и использовать Codex **после собственной реализации** как строгого ревьюера. План специально устроен так, чтобы не читать Qt «на будущее»: перед каждой задачей ты изучаешь только небольшой набор технологий, сразу применяешь их и закрепляешь на работающем коде.

## Правила

- Не читай Qt-документацию подряд.
- Открывай только ссылки из текущей задачи в `04_TASKS.md`.
- Перед кодом на 3–5 строк ответь себе: **где живут данные, кто ими владеет, в каком потоке выполняется код**.
- Сначала сделай собственную попытку.
- Если застрял, сначала сделай минимальный эксперимент или прочитай документацию конкретного класса.
- Не проси Codex написать задачу целиком до своей попытки.
- Перед review проект должен собираться, приложение — запускаться, тесты текущего этапа — проходить.
- После исправлений делай повторный review.
- После каждой задачи запиши 2–4 предложения: что ты теперь понимаешь и что было неожиданным.

## Цикл одной задачи

1. Прочитать раздел «Технологии» и 2–4 ссылки.
2. Сформулировать ожидаемый результат задачи.
3. Реализовать его самому.
4. Запустить build/tests.
5. Локализовать очевидные проблемы самому.
6. Отдать diff в Codex в режиме review only.
7. Самостоятельно исправить найденное.
8. Сделать re-review.
9. Отметить Definition of Done.
10. Перейти дальше.

## Базовый prompt для Codex

```text
Review only. Do not modify files and do not implement fixes.

Review my current diff plus the surrounding code needed to understand it.
Prioritize correctness over style. For each finding provide:
1) severity, 2) file/line, 3) why it is wrong, 4) a concrete failure scenario,
5) the direction of the fix, but no full patch unless I explicitly ask.

Always check: QObject ownership/lifetime, thread affinity, event-loop blocking,
direct vs queued signal delivery, data races, unsafe captures, C++/QML boundary,
QAbstractItemModel contract, error paths, shutdown/cancellation, and missing tests.
If you find no correctness issues, say that explicitly and list your assumptions.
```

## Рекомендуемый `AGENTS.md`

```markdown
# Learning-project rules
- This is a C++/Qt learning project. The developer writes features manually.
- Default to review/diagnostic mode. Do not modify files unless explicitly asked.
- For reviews, explain bugs and failure scenarios instead of writing the patch.
- Qt-specific review priorities: QObject lifetime, parent ownership, thread affinity,
  event loops, queued/direct connections, GUI-thread rules, model/view invariants,
  C++/QML ownership, cancellation and clean shutdown.
- Run the relevant build/tests when available and report exactly what was run.
- Do not replace an intentional Qt mechanism with an unrelated abstraction without
  explaining the trade-off first.
```

## Что Codex должен искать во всём проекте

- GUI или `QAbstractItemModel` меняется из worker/pool thread.
- `QObject` создан в одном потоке, но используется так, будто принадлежит другому.
- Прямой вызов метода через границу потоков вместо queued handoff.
- Blocking wait, `future.result()` или `waitForReadyRead()` в GUI thread.
- `this` захвачен задачей, которая может закончиться после уничтожения owner.
- `QThread` уничтожается раньше managed thread.
- Worker/socket удаляется в неправильном потоке.
- Очередь, history или task backlog растёт без верхней границы.
- Один signal/task на каждый крошечный packet при высокой нагрузке без batching.
- Model мутируется без `begin.../end...` или корректного `dataChanged`.
- C++ напрямую лазит по QML object tree.
- Cancel есть в UI, но worker его реально не проверяет.
- Async-тест проходит только из-за `sleep` и удачного тайминга.

## Чекпоинт знаний после проекта

| Тема | Ты должен ответить без подсказки |
|---|---|
| QObject | Кто владеет объектом? Что делает parent? Когда нужен `deleteLater()`? |
| Meta-object | Что даёт `Q_OBJECT`/moc? Как работают signals/slots/properties поверх C++? |
| Signals/slots | Когда `AutoConnection` становится queued? В каком thread выполняется slot? |
| Event loop | Почему `QTimer`/socket требуют event loop? Что блокирует GUI? |
| QThread | Где живёт объект `QThread`? Зачем worker-object pattern? |
| Thread affinity | Почему наличие pointer не даёт права вызывать `QObject` из любого thread? |
| Model/View | Какие notifications обязательны при insert/update? |
| QML boundary | Почему C++ API должен быть стабильным и не зависеть от QML ids? |
| QThreadPool | Почему короткие CPU jobs подходят pool? |
| Backpressure | Что происходит, если producer быстрее consumer? |
| QtConcurrent | Как progress/cancel доставляются в UI без блокировки? |
| Shutdown | Как выглядит точный safe sequence остановки? |
| Testing | Как `QSignalSpy`/`QTRY_*` заменяют случайные sleeps? |
| Performance | Какими измерениями доказано, что batching/backpressure помогли? |

## Прогресс

- [ ] 0 — Bootstrap/CMake
- [ ] 1 — QObject/meta-object/signals
- [ ] 2 — QML shell + Dashboard
- [ ] 3 — Device model + list/detail
- [ ] 4 — Qt Test infrastructure
- [ ] 5 — Simulator + UDP + Add Source
- [ ] 6 — QThread worker + affinity
- [ ] 7 — Graceful shutdown + connection error UI
- [ ] 8 — Batching + Dashboard live data
- [ ] 9 — QThreadPool + statistics + charts
- [ ] 10 — QtConcurrent export
- [ ] 11 — User event journal + diagnostics
- [ ] 12 — Persistent settings
- [ ] 13 — Load testing/profiling
- [ ] 14 — Concurrency/failure tests
- [ ] 15 — Финальная упаковка

## Ссылки по Codex

- https://openai.com/business/guides-and-resources/how-openai-uses-codex/
- https://openai.com/index/introducing-codex/
