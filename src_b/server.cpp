/**
 * @file server.cpp
 * @brief Implementation of the BLINK DB server
 * @author Your Name
 * @date March 31, 2025
 */

 #include "server.h"
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <sys/epoll.h>
 #include <iostream>
 #include <cstring>
 #include <errno.h>
 #include <algorithm>
 
 /**
  * @brief Set socket to non-blocking mode
  * @param sockfd Socket file descriptor to set as non-blocking
  */
 void setNonBlocking(int sockfd) {
     int flags = fcntl(sockfd, F_GETFL, 0);
     fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
 }
 
 /**
  * @brief Constructor for Server
  * @param port Port number to listen on
  * @param engine Shared pointer to the storage engine
  */
 Server::Server(int port, std::shared_ptr<StorageEngine> engine)
     : port_(port), server_fd_(-1), epoll_fd_(-1), engine_(engine), running_(false) {}
 
 /**
  * @brief Destructor for Server
  * 
  * Cleans up resources by closing server socket, epoll instance,
  * and all client connections.
  */
 Server::~Server() {
     if (server_fd_ >= 0) {
         close(server_fd_);
     }
     
     if (epoll_fd_ >= 0) {
         close(epoll_fd_);
     }
     
     // Close all client connections
     for (auto& client : clients_) {
         close(client.first);
     }
 }
 
 /**
  * @brief Start the server
  * @return 0 on successful exit, non-zero on error
  * 
  * Initializes the server socket and epoll instance, then enters
  * the main event loop to handle client connections and requests.
  */
 int Server::start() {
     if (!initServerSocket() || !initEpoll()) {
         return 1;
     }
     
     running_ = true;
     std::cout << "Server started on port " << port_ << std::endl;
     
     const int MAX_EVENTS = 64;
     struct epoll_event events[MAX_EVENTS];
     
     while (running_) {
         int num_events = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
         
         if (num_events < 0) {
             if (errno == EINTR) {
                 continue;  // Interrupted by signal, continue
             }
             std::cerr << "epoll_wait error: " << strerror(errno) << std::endl;
             break;
         }
         
         for (int i = 0; i < num_events; i++) {
             int fd = events[i].data.fd;
             
             if (fd == server_fd_) {
                 // New connection
                 acceptClient();
             } else {
                 // Existing client data
                 if (events[i].events & EPOLLIN) {
                     handleClient(fd);
                 }
                 
                 if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                     closeClient(fd);
                 }
             }
         }
     }
     
     return 0;
 }
 
 /**
  * @brief Initialize the server socket
  * @return true if successful, false otherwise
  * 
  * Creates a socket, sets socket options, binds to the specified port,
  * and starts listening for connections.
  */
 bool Server::initServerSocket() {
     server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
     if (server_fd_ < 0) {
         std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
         return false;
     }
     
     // Allow reuse of address
     int opt = 1;
     if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
         std::cerr << "setsockopt failed: " << strerror(errno) << std::endl;
         close(server_fd_);
         return false;
     }
     
     // Set non-blocking mode
     setNonBlocking(server_fd_);
     
     // Bind to port
     struct sockaddr_in address;
     address.sin_family = AF_INET;
     address.sin_addr.s_addr = INADDR_ANY;
     address.sin_port = htons(port_);
     
     if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
         std::cerr << "Bind failed: " << strerror(errno) << std::endl;
         close(server_fd_);
         return false;
     }
     
     // Start listening
     if (listen(server_fd_, SOMAXCONN) < 0) {
         std::cerr << "Listen failed: " << strerror(errno) << std::endl;
         close(server_fd_);
         return false;
     }
     
     return true;
 }
 
 /**
  * @brief Initialize epoll
  * @return true if successful, false otherwise
  * 
  * Creates an epoll instance and adds the server socket to it.
  */
 bool Server::initEpoll() {
     epoll_fd_ = epoll_create1(0);
     if (epoll_fd_ < 0) {
         std::cerr << "Failed to create epoll instance: " << strerror(errno) << std::endl;
         return false;
     }
     
     // Add server socket to epoll
     struct epoll_event event;
     event.events = EPOLLIN | EPOLLET;  // Edge-triggered mode
     event.data.fd = server_fd_;
     
     if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, server_fd_, &event) < 0) {
         std::cerr << "Failed to add server socket to epoll: " << strerror(errno) << std::endl;
         close(epoll_fd_);
         return false;
     }
     
     return true;
 }
 
 /**
  * @brief Accept a new client connection
  * 
  * Accepts new client connections, sets them to non-blocking mode,
  * and adds them to the epoll instance.
  */
 void Server::acceptClient() {
     struct sockaddr_in client_addr;
     socklen_t client_len = sizeof(client_addr);
     
     while (true) {
         int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_len);
         
         if (client_fd < 0) {
             if (errno == EAGAIN || errno == EWOULDBLOCK) {
                 // No more connections to accept
                 break;
             } else {
                 std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                 break;
             }
         }
         
         // Set client socket to non-blocking mode
         setNonBlocking(client_fd);
         
         // Add client socket to epoll
         struct epoll_event event;
         event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;  // Edge-triggered mode
         event.data.fd = client_fd;
         
         if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &event) < 0) {
             std::cerr << "Failed to add client socket to epoll: " << strerror(errno) << std::endl;
             close(client_fd);
             continue;
         }
         
         // Create client context
         clients_[client_fd] = ClientContext{client_fd, "", RespProtocol()};
         
         std::cout << "New client connected: " << client_fd << std::endl;
     }
 }
 
 /**
  * @brief Handle data from a client
  * @param client_fd Client file descriptor
  * 
  * Reads data from a client, parses RESP commands, and processes them.
  */
 void Server::handleClient(int client_fd) {
     auto it = clients_.find(client_fd);
     if (it == clients_.end()) {
         return;
     }
     
     char buffer[4096];
     ssize_t bytes_read;
     
     // Read all available data (required for edge-triggered mode)
     while (true) {
         bytes_read = read(client_fd, buffer, sizeof(buffer));
         
         if (bytes_read < 0) {
             if (errno == EAGAIN || errno == EWOULDBLOCK) {
                 // No more data to read
                 break;
             } else {
                 // Error occurred
                 std::cerr << "Read error: " << strerror(errno) << std::endl;
                 closeClient(client_fd);
                 return;
             }
         } else if (bytes_read == 0) {
             // Client closed connection
             closeClient(client_fd);
             return;
         }
         
         // Append data to client buffer
         it->second.buffer.append(buffer, bytes_read);
         
         // Try to parse RESP commands
         std::vector<std::string> command = it->second.protocol.parseRequest(it->second.buffer);
         
         if (!command.empty()) {
             // Clear the processed part of the buffer
             it->second.buffer.clear();
             
             // Process the command
             processCommand(client_fd, command);
         }
     }
 }
 
 /**
  * @brief Close a client connection
  * @param client_fd Client file descriptor
  * 
  * Removes the client from epoll, closes the socket, and cleans up resources.
  */
 void Server::closeClient(int client_fd) {
     std::cout << "Client disconnected: " << client_fd << std::endl;
     
     // Remove from epoll
     epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, nullptr);
     
     // Close socket
     close(client_fd);
     
     // Remove from clients map
     clients_.erase(client_fd);
 }
 
 /**
  * @brief Process a command from a client
  * @param client_fd Client file descriptor
  * @param command The command to process
  * 
  * Processes a RESP command (SET, GET, DEL) and sends the response back to the client.
  */
 void Server::processCommand(int client_fd, const std::vector<std::string>& command) {
     if (command.empty()) {
         return;
     }
     
     auto it = clients_.find(client_fd);
     if (it == clients_.end()) {
         return;
     }
     
     std::string cmd = command[0];
     std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
     
     std::string response;
     
     if (cmd == "SET" && command.size() >= 3) {
         engine_->set(command[1], command[2]);
         response = it->second.protocol.encodeSimpleString("OK");
     } else if (cmd == "CONFIG") {
        response = it->second.protocol.encodeArray({}); // or some minimal config info
     } else if (cmd == "GET" && command.size() >= 2) {
         std::string value = engine_->get(command[1]);
         if (value == "NULL") {
             response = it->second.protocol.encodeNull();
         } else {
             response = it->second.protocol.encodeBulkString(value);
         }
     } else if (cmd == "DEL" && command.size() >= 2) {
         bool success = engine_->del(command[1]);
         if (success) {
             response = it->second.protocol.encodeInteger(1);
         } else {
             response = it->second.protocol.encodeInteger(0);
         }
     }
    else {
         response = it->second.protocol.encodeError("ERR unknown command or wrong number of arguments");
     }
     
     // Send response
     if (!response.empty()) {
         ssize_t bytes_sent = write(client_fd, response.c_str(), response.size());
         if (bytes_sent < 0) {
             std::cerr << "Write error: " << strerror(errno) << std::endl;
             closeClient(client_fd);
         }
     }
 }
 