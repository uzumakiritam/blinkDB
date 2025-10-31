
# BlinkDB: A Redis-Inspired In-Memory Key-Value Database

## Overview
BlinkDB is a high-performance, Redis-inspired in-memory key-value database written in modern C++17.  
It supports persistence, TTL expirations, LRU eviction, and the Redis RESP2 wire protocol.  
BlinkDB offers high throughput for core operations (`SET`, `GET`, `DEL`) and remains lightweight and easy to deploy.

---

## System Design
BlinkDB consists of three main components:
1. **Storage Engine** – Handles CRUD operations, TTL, persistence (AOF + snapshots), and statistics.
2. **RESP2 Protocol Module** – Implements Redis-compatible serialization and parsing.
3. **Server Layer** – A TCP server using Linux `epoll` for concurrent client handling.

---

## Key Features
- **Persistence:** Append-only logging (AOF) and periodic RDB-like snapshots.  
- **Eviction:** Optional LRU policy via environment variable:
```
  BLINKMAXMEM=134217728 ./bin/blinkdb   # 128MB limit
```

* **RESP2 Compatibility:** Works seamlessly with `redis-cli` and `redis-benchmark`.
* **Supported Commands:** `SET`, `GET`, `DEL`, `EXPIRE`, `TTL`, `FLUSHDB`, `SAVE`, `STATS`, `KEYS`, `PING`.

---

## Build & Run

```
make clean && make
./bin/blinkdb           # default port 9001
./bin/blinkdb 6380      # custom port
```

Connect via Redis CLI:

```
redis-cli -p 9001
127.0.0.1:9001> SET user:1 Alice
127.0.0.1:9001> GET user:1
"Alice"
127.0.0.1:9001> STATS
```

---

## Benchmark Summary

Measured using `redis-benchmark`:

| Requests  | Conns | Throughput (ops/s) | Avg Lat (ms) |
| --------- | ----- | ------------------ | ------------ |
| 100,000   | 10    | 77,942             | 0.07         |
| 100,000   | 100   | 64,566             | 0.85         |
| 1,000,000 | 1000  | 51,551             | 9.77         |

BlinkDB achieves sub-millisecond latency under moderate load and competitive throughput at scale.

---

## Persistence Demo
```
./bin/blinkdb &
redis-cli -p 9001 SET user:1 Alice
redis-cli -p 9001 SAVE
kill <PID>
./bin/blinkdb &
redis-cli -p 9001 GET user:1   # returns "Alice"
```
