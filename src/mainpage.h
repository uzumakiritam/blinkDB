/**
 * @mainpage BLINK DB Storage Engine
 * 
 * @section intro_sec Introduction
 * BLINK DB is a high-performance key-value in-memory database inspired by Redis.
 * This documentation covers Part A of the project: the storage engine implementation.
 * 
 * @section design_sec Design Decisions
 * 
 * @subsection workload_sec Workload Optimization
 * This implementation is optimized for balanced workloads where reads and writes are comparable.
 * 
 * @subsection datastructure_sec Data Structure
 * The storage engine uses a hash table (unordered_map) as its primary data structure, providing:
 * - O(1) average time complexity for all operations (GET, SET, DEL)
 * - Efficient memory usage
 * - Simple implementation
 * 
 * @subsection memory_sec Memory Management
 * To prevent memory overflow, an LRU (Least Recently Used) cache eviction policy is implemented:
 * - Recently accessed items are kept in memory
 * - Least recently used items are evicted when memory limits are reached
 * - Memory usage is tracked for each item
 * 
 * @section usage_sec Usage
 * The storage engine provides three main operations:
 * - set(key, value): Store a value with the given key
 * - get(key): Retrieve the value associated with the key
 * - del(key): Delete the key-value pair
 * 
 * @section repl_sec REPL Interface
 * A simple REPL interface is provided for interacting with the storage engine:
 * - SET <key> "<value>"
 * - GET <key>
 * - DEL <key>
 */
