/**
 * @file storage_engine.h
 * @brief Header file for the BLINK DB storage engine
 * 
 * This file contains the declaration of the StorageEngine class
 * which provides the core functionality for the key-value database.
 */

 #ifndef STORAGE_ENGINE_H
 #define STORAGE_ENGINE_H
 
 #include <string>
 #include <unordered_map>
 #include <list>
 #include <mutex>
 #include <chrono>
 
 /**
  * @class StorageEngine
  * @brief Core storage engine for BLINK DB
  * 
  * Implements a key-value storage with LRU cache eviction policy
  * for efficient memory management.
  */
 class StorageEngine {
 public:
     /**
      * @brief Constructor for StorageEngine
      * @param max_memory_size Maximum memory size in bytes (default: 1GB)
      */
     StorageEngine(size_t max_memory_size = 1024 * 1024 * 1024);
     
     /**
      * @brief Set a key-value pair in the database
      * @param key The key to set
      * @param value The value to associate with the key
      * @return true if successful, false otherwise
      */
     bool set(const std::string& key, const std::string& value);
     
     /**
      * @brief Get the value associated with a key
      * @param key The key to look up
      * @return The value associated with the key, or "NULL" if not found
      */
     std::string get(const std::string& key);
     
     /**
      * @brief Delete a key-value pair from the database
      * @param key The key to delete
      * @return true if the key was found and deleted, false otherwise
      */
     bool del(const std::string& key);
     
     /**
      * @brief Get the current memory usage
      * @return Current memory usage in bytes
      */
     size_t getMemoryUsage() const;
 
 private:
     /**
      * @struct CacheItem
      * @brief Structure to store cache items with metadata
      */
     struct CacheItem {
         std::string value;
         std::chrono::steady_clock::time_point last_accessed;
         size_t size;
     };
     
     std::unordered_map<std::string, CacheItem> data_store_;
     std::list<std::string> lru_list_;
     std::unordered_map<std::string, std::list<std::string>::iterator> lru_map_;
     
     size_t max_memory_size_;
     size_t current_memory_usage_;
     std::mutex mutex_;
     
     /**
      * @brief Update the LRU list when a key is accessed
      * @param key The key that was accessed
      */
     void updateLRU(const std::string& key);
     
     /**
      * @brief Evict items from cache if memory limit is reached
      * @param required_size The size needed for a new item
      */
     void evictIfNeeded(size_t required_size);
     
     /**
      * @brief Calculate the memory size of a key-value pair
      * @param key The key
      * @param value The value
      * @return Size in bytes
      */
     size_t calculateItemSize(const std::string& key, const std::string& value) const;
 };
 
 #endif // STORAGE_ENGINE_H
 