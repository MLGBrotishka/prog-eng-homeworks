# Оптимизация производительности PostgreSQL

В данном документе представлен детальный анализ производительности базы данных системы такси, включая планы выполнения ключевых запросов (EXPLAIN) и обоснование выбора индексов.

## 1. Стратегия индексирования

Для обеспечения производительности O(log N) для основных операций API были созданы следующие B-Tree индексы:

- `idx_users_login` (таблица `users`, колонка `login`) — для быстрого поиска пользователя при аутентификации.
- `idx_users_name` (таблица `users`, колонки `first_name`, `last_name`) — для поиска пользователей по имени.
- `idx_drivers_user_id` (таблица `drivers`, колонка `user_id`) — для связи профиля водителя с пользователем.
- `idx_drivers_status` (таблица `drivers`, колонка `status`) — для фильтрации доступных водителей.
- `idx_rides_passenger_id` (таблица `rides`, колонка `passenger_id`) — для получения истории поездок пассажира.
- `idx_rides_driver_id` (таблица `rides`, колонка `driver_id`) — для получения истории поездок водителя.
- `idx_rides_status` (таблица `rides`, колонка `status`) — для отображения активных поездок.
- `idx_rides_created_at` (таблица `rides`, колонка `created_at`) — для быстрой сортировки поездок по времени без дополнительных затрат CPU/RAM.

## 2. Анализ планов выполнения запросов (EXPLAIN ANALYZE)

> **Примечание:** В рамках ДЗ 04 сущность "Пользователь" (таблица `users`) была мигрирована в **MongoDB**. Приведенные ниже профилировки для `users` актуальны для исторической версии системы на PostgreSQL (ДЗ 03).

### 2.1 Поиск пользователя по логину (API Op 2)

**Запрос:**
```sql
EXPLAIN ANALYZE SELECT id, login, email, first_name, last_name, password_hash, created_at 
FROM users WHERE login = 'john_doe';
```

**План выполнения:**
```text
Index Scan using idx_users_login on users  (cost=0.14..8.16 rows=1 width=1354) (actual time=0.015..0.016 rows=1 loops=1)
  Index Cond: ((login)::text = 'john_doe'::text)
Planning Time: 0.068 ms
Execution Time: 0.034 ms
```
**Вывод:** Используется индекс `idx_users_login`. Поиск выполняется за константное время O(log N), что идеально для частой операции входа в систему.

### 2.2 Поиск истории поездок пассажира (API Op 8)

**Запрос:**
```sql
EXPLAIN ANALYZE SELECT id, passenger_id, driver_id, from_address, to_address, status, cost, created_at 
FROM rides WHERE passenger_id = 1 ORDER BY created_at DESC;
```

**План выполнения:**
```text
Index Scan Backward using idx_rides_created_at on rides  (cost=0.14..8.21 rows=3 width=116) (actual time=0.018..0.021 rows=3 loops=1)
  Filter: (passenger_id = 1)
Planning Time: 0.084 ms
Execution Time: 0.042 ms
```
**Вывод:** Оптимизатор выбрал индекс по времени создания (`idx_rides_created_at`) для сканирования в обратном порядке, так как данных мало. На больших объемах данных с тем же распределением будет использоваться `idx_rides_passenger_id` с последующей сортировкой или составной индекс. Текущая производительность высока.

### 2.3 Поиск пользователей по маске имени (API Op 3)

**Запрос:**
```sql
EXPLAIN ANALYZE SELECT id, login, email, first_name, last_name, password_hash, created_at 
FROM users WHERE first_name || ' ' || last_name ILIKE '%John%';
```

**План выполнения:**
```text
Seq Scan on users  (cost=0.00..1.15 rows=1 width=1354) (actual time=0.012..0.014 rows=1 loops=1)
  Filter: (((first_name)::text || ' ' || (last_name)::text) ~~* '%John%'::text)
  Rows Removed by Filter: 11
Planning Time: 0.075 ms
Execution Time: 0.035 ms
```
**Вывод:** В текущей реализации используется последовательное сканирование (`Seq Scan`), так как индекс `idx_users_name(first_name, last_name)` не может быть использован для поиска по конкатенации строк с `ILIKE`. 
*Рекомендация:* Для оптимизации на больших таблицах следует использовать GIN индекс с расширением `pg_trgm` или функциональный индекс на `lower(first_name || ' ' || last_name)`.

## 3. Масштабируемость

Для таблицы `rides`, которая является самой быстрорастущей, предусмотрено горизонтальное масштабирование через **Range Partitioning** по полю `created_at`. Это позволит:
1. Быстро удалять старые данные (DROP PARTITION вместо DELETE).
2. Повысить производительность запросов за счет исключения неактуальных разделов (Partition Pruning).
3. Распределять разделы по разным физическим дискам.
