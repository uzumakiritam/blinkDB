/**
 * @file main.cpp
 * @brief Main entry point for BLINK DB server
 * 
 * This file contains the main function that initializes and starts
 * the BLINK DB server.
 */

 #include "storage_engine.h"
 #include "server.h"
 #include <iostream>
 #include <memory>
 #include <signal.h>
 
 // Global server pointer for signal handling
 std::shared_ptr<Server> g_server;
 
 /**
  * @brief Signal handler for graceful shutdown
  * @param sig Signal number
  */
 void signalHandler(int sig) {
     std::cout << "\nReceived signal " << sig << ", shutting down..." << std::endl;
     
     // Clean up and exit
     g_server.reset();
     exit(0);
 }
 
 /**
  * @brief Print usage information
  * @param progName Program name
  */
 void printUsage(const char* progName) {
     std::cout << "Usage: " << progName << " [PORT]" << std::endl;
     std::cout << "  PORT - Port number to listen on (default: 9001)" << std::endl;
 }
 
 /**
  * @brief Main function
  * @param argc Argument count
  * @param argv Argument vector
  * @return Exit code
  */
 int main(int argc, char* argv[]) {
     // Set up signal handlers
     signal(SIGINT, signalHandler);
     signal(SIGTERM, signalHandler);
     
     // Parse command line arguments
     int port = 9001;  // Default port
     
     if (argc > 1) {
         try {
             port = std::stoi(argv[1]);
             if (port <= 0 || port > 65535) {
                 std::cerr << "Invalid port number. Port must be between 1 and 65535." << std::endl;
                 return 1;
             }
         } catch (const std::exception& e) {
             std::cerr << "Invalid port number: " << argv[1] << std::endl;
             printUsage(argv[0]);
             return 1;
         }
     }
     
     // Create storage engine
     std::shared_ptr<StorageEngine> engine = std::make_shared<StorageEngine>();
     
     // Create and start server
     g_server = std::make_shared<Server>(port, engine);
     
     std::cout << "Starting BLINK DB server on port " << port << "..." << std::endl;
     return g_server->start();
 }
 