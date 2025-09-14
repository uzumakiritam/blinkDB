/**
 * @file storage_engine.cpp
 * @brief Implementation of the BLINK DB storage engine
 * @author Your Name
 * @date March 31, 2025
 */

 #include "storage_engine.h"
 #include <iostream>
 
 /**
  * @brief Constructor for StorageEngine
  * @param max_memory_size Maximum memory size in bytes
  */
 StorageEngine::StorageEngine(size_t max_memory_size)
     : max_memory_size_(max_memory_size), current_memory_usage_(0) {}
 
 /**
  * @brief Set a key-value pair in the database
  * @param key The key to set
  * @param value The value to associate with the key
  * @return true if successful, false otherwise
  */
 bool StorageEngine::set(const std::string& key, const std::string& value) {
     std::lock_guard<std::mutex> lock(mutex_);
     
     size_t new_item_size = calculateItemSize(key, value);
     
     // If key exists, update its value and adjust memory usage
     auto it = data_store_.find(key);
     if (it != data_store_.end()) {
         size_t old_size = it->second.size;
         current_memory_usage_ -= old_size;
         current_memory_usage_ += new_item_size;
         
         it->second.value = value;
         it->second.last_accessed = std::chrono::steady_clock::now();
         it->second.size = new_item_size;
         
         updateLRU(key);
         return true;
     }
     
     // Check if we need to evict items
     evictIfNeeded(new_item_size);
     
     // Insert new item
     CacheItem item{
         value,
         std::chrono::steady_clock::now(),
         new_item_size
     };
     
     data_store_[key] = item;
     current_memory_usage_ += new_item_size;
     
     // Update LRU
     lru_list_.push_front(key);
     lru_map_[key] = lru_list_.begin();
     
     return true;
 }
 
 /**
  * @brief Get the value associated with a key
  * @param key The key to look up
  * @return The value associated with the key, or "NULL" if not found
  */
 std::string StorageEngine::get(const std::string& key) {
     std::lock_guard<std::mutex> lock(mutex_);
     
     auto it = data_store_.find(key);
     if (it != data_store_.end()) {
         it->second.last_accessed = std::chrono::steady_clock::now();
         updateLRU(key);
         return it->second.value;
     }
     
     return "NULL";
 }
 
 /**
  * @brief Delete a key-value pair from the database
  * @param key The key to delete
  * @return true if the key was found and deleted, false otherwise
  */
 bool StorageEngine::del(const std::string& key) {
     std::lock_guard<std::mutex> lock(mutex_);
     
     auto it = data_store_.find(key);
     if (it != data_store_.end()) {
         current_memory_usage_ -= it->second.size;
         
         // Remove from LRU tracking
         auto lru_it = lru_map_.find(key);
         if (lru_it != lru_map_.end()) {
             lru_list_.erase(lru_it->second);
             lru_map_.erase(lru_it);
         }
         
         // Remove from data store
         data_store_.erase(it);
         return true;
     }
     
     return false;
 }
 
 /**
  * @brief Get the current memory usage
  * @return Current memory usage in bytes
  */
 size_t StorageEngine::getMemoryUsage() const {
     return current_memory_usage_;
 }
 
 /**
  * @brief Update the LRU list when a key is accessed
  * @param key The key that was accessed
  */
 void StorageEngine::updateLRU(const std::string& key) {
     auto it = lru_map_.find(key);
     if (it != lru_map_.end()) {
         lru_list_.erase(it->second);
         lru_list_.push_front(key);
         it->second = lru_list_.begin();
     }
 }
 
 /**
  * @brief Evict items from cache if memory limit is reached
  * @param required_size The size needed for a new item
  */
 void StorageEngine::evictIfNeeded(size_t required_size) {
     // If we don't have enough memory, evict items using LRU policy
     while (!lru_list_.empty() && (current_memory_usage_ + required_size > max_memory_size_)) {
         std::string oldest_key = lru_list_.back();
         auto it = data_store_.find(oldest_key);
         
         if (it != data_store_.end()) {
             current_memory_usage_ -= it->second.size;
             data_store_.erase(it);
         }
         
         lru_list_.pop_back();
         lru_map_.erase(oldest_key);
     }
 }
 
 /**
  * @brief Calculate the memory size of a key-value pair
  * @param key The key
  * @param value The value
  * @return Size in bytes
  */
 size_t StorageEngine::calculateItemSize(const std::string& key, const std::string& value) const {
     // Size calculation: key size + value size + overhead (estimated)
     // Overhead includes hash table entry, LRU tracking, etc.
     const size_t OVERHEAD_PER_ENTRY = 64;  // Estimated overhead in bytes
     return key.size() + value.size() + OVERHEAD_PER_ENTRY;
 }
 