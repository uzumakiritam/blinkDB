# BlinkDB: A Redis-Inspired High-Performance In-Memory Key-Value Database

## Overview

BlinkDB is a high-performance, Redis-inspired in-memory key-value database implemented in modern C++17.  
It supports persistent storage, TTL expirations, LRU eviction, and the Redis RESP2 wire protocol.  
BlinkDB is designed for high throughput while remaining lightweight and easy to deploy as a standalone server.

---

## System Design

BlinkDB is structured into three primary components:

1. **Storage Engine**  
   Handles CRUD operations, TTL management, persistence through append-only files (AOF) and snapshotting, and statistics collection.

2. **RESP2 Protocol Module**  
   Encodes and decodes client requests and responses using the Redis Serialization Protocol, ensuring full compatibility with Redis clients such as `redis-cli` and `redis-benchmark`.

3. **Server Layer**  
   A TCP server built using Linux `epoll` for concurrent I/O multiplexing.  
   Manages multiple client connections and dispatches commands to the storage engine.

---

## Persistence Mechanism

BlinkDB supports two complementary persistence methods:

- **Append-Only File (AOF)**: Logs every write/delete command for crash recovery.  
- **Snapshotting (RDB-like)**: Periodically serializes the in-memory dataset to a file (`blink.rdb`).  
- On restart, the system loads the snapshot and replays AOF logs to restore state.

---

## Memory Management and Eviction

- Optional **LRU eviction** can be enabled with:
  ```bash
  BLINKMAXMEM=<bytes> ./bin/blinkdb
