/**
 * @file server.h
 * @brief Header file for BLINK DB server
 * 
 * This file contains the declaration of the Server class that
 * implements a TCP server with epoll-based event handling.
 */

 #ifndef SERVER_H
 #define SERVER_H
 
 #include "storage_engine.h"
 #include "resp_protocol.h"
 #include <unordered_map>
 #include <string>
 #include <vector>
 #include <memory>
 
 /**
  * @class Server
  * @brief TCP server implementation for BLINK DB
  * 
  * Implements a TCP server that handles multiple client connections
  * using epoll for I/O multiplexing.
  */
 class Server {
 public:
     /**
      * @brief Constructor for Server
      * @param port Port number to listen on
      * @param engine Pointer to the storage engine
      */
     Server(int port, std::shared_ptr<StorageEngine> engine);
     
     /**
      * @brief Destructor for Server
      */
     ~Server();
     
     /**
      * @brief Start the server
      * @return 0 on successful exit, non-zero on error
      */
     int start();
 
 private:
     /**
      * @struct ClientContext
      * @brief Structure to store client connection context
      */
     struct ClientContext {
         int fd;
         std::string buffer;
         RespProtocol protocol;
     };
     
     int port_;
     int server_fd_;
     int epoll_fd_;
     std::shared_ptr<StorageEngine> engine_;
     std::unordered_map<int, ClientContext> clients_;
     bool running_;
     
     /**
      * @brief Initialize the server socket
      * @return true if successful, false otherwise
      */
     bool initServerSocket();
     
     /**
      * @brief Initialize epoll
      * @return true if successful, false otherwise
      */
     bool initEpoll();
     
     /**
      * @brief Accept a new client connection
      */
     void acceptClient();
     
     /**
      * @brief Handle data from a client
      * @param client_fd Client file descriptor
      */
     void handleClient(int client_fd);
     
     /**
      * @brief Close a client connection
      * @param client_fd Client file descriptor
      */
     void closeClient(int client_fd);
     
     /**
      * @brief Process a command from a client
      * @param client_fd Client file descriptor
      * @param command The command to process
      */
     void processCommand(int client_fd, const std::vector<std::string>& command);
 };
 
 #endif // SERVER_H
 