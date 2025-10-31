/**
 * @mainpage BLINK DB Network Infrastructure
 * 
 * @section intro_sec Introduction
 * BLINK DB is a high-performance key-value in-memory database inspired by Redis.
 * This documentation covers Part B of the project: the network infrastructure implementation
 * that transforms the storage engine into a complete client-server database system.
 * 
 * @section arch_sec Architecture
 * The network infrastructure consists of several key components:
 * - TCP Server: Handles client connections and network I/O
 * - RESP Protocol: Implements the Redis Serialization Protocol (RESP-2)
 * - Connection Management: Uses epoll for efficient I/O multiplexing
 * - Integration with Storage Engine: Connects network layer to the database backend
 * 
 * @section design_sec Design Decisions
 * 
 * @subsection conn_sec Connection Management
 * This implementation uses epoll with edge-triggered mode for efficient connection handling:
 * - Single-threaded event loop processes multiple client connections concurrently
 * - Non-blocking I/O prevents any single client from blocking the server
 * - Edge-triggered notifications reduce system call overhead
 * 
 * @subsection proto_sec Protocol Implementation
 * The RESP-2 protocol implementation supports:
 * - Simple strings (+)
 * - Errors (-)
 * - Integers (:)
 * - Bulk strings ($)
 * - Arrays (*)
 * 
 * @subsection perf_sec Performance Considerations
 * The server is optimized for:
 * - Minimizing memory allocations during request processing
 * - Efficient buffer management for partial reads
 * - Quick command dispatching to storage engine
 * 
 * @section benchmark_sec Benchmark Results
 * Performance benchmarks using redis-benchmark show:
 * - SET: ~76,000 operations per second with 1000 parallel connections
 * - GET: ~76,000 operations per second with 1000 parallel connections
 * 
 * @section usage_sec Usage
 * The server listens on port 9001 by default and supports the following Redis commands:
 * - SET \<key\> \<value\>
 * - GET \<key\>
 * - DEL \<key\>
 * 
 * @section build_sec Building and Running
 * To build and run the server:
 * ```
 * make
 * ./bin/blink_db [PORT]
 * ```
 * 
 * To benchmark the server:
 * ```
 * make benchmark
 * ```
 */
