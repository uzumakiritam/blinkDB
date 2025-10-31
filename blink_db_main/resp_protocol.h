/**
 * @file resp_protocol.h
 * @brief Header file for RESP (Redis Serialization Protocol) implementation
 * 
 * This file contains the declaration of the RespProtocol class that
 * implements RESP-2 protocol for client-server communication.
 */

 #ifndef RESP_PROTOCOL_H
 #define RESP_PROTOCOL_H
 
 #include <string>
 #include <vector>
 #include <sstream>
 
 /**
  * @class RespProtocol
  * @brief Implementation of RESP-2 protocol
  * 
  * Provides methods for encoding and decoding messages using the
  * RESP-2 protocol for communication with Redis clients.
  */
 class RespProtocol {
 public:
     /**
      * @brief Default constructor
      */
     RespProtocol() = default;
     
     /**
      * @brief Parse RESP data from a buffer
      * @param buffer The buffer containing RESP data
      * @return A vector of strings representing the parsed command
      */
     std::vector<std::string> parseRequest(const std::string& buffer);
     
     /**
      * @brief Encode a simple string response
      * @param str The string to encode
      * @return RESP-encoded string
      */
     std::string encodeSimpleString(const std::string& str);
     
     /**
      * @brief Encode an error response
      * @param err The error message to encode
      * @return RESP-encoded error
      */
     std::string encodeError(const std::string& err);
     
     /**
      * @brief Encode an integer response
      * @param num The integer to encode
      * @return RESP-encoded integer
      */
     std::string encodeInteger(int64_t num);
     
     /**
      * @brief Encode a bulk string response
      * @param str The string to encode
      * @return RESP-encoded bulk string
      */
     std::string encodeBulkString(const std::string& str);
     
     /**
      * @brief Encode a null bulk string
      * @return RESP-encoded null bulk string
      */
     std::string encodeNull();
     
     /**
      * @brief Encode an array response
      * @param arr The array of strings to encode
      * @return RESP-encoded array
      */
     std::string encodeArray(const std::vector<std::string>& arr);
     
 private:
     /**
      * @brief Parse a RESP array
      * @param input The input stream containing RESP data
      * @param count The number of elements in the array
      * @return A vector of strings representing the parsed array
      */
     std::vector<std::string> parseArray(std::istringstream& input, int count);
     
     /**
      * @brief Parse a RESP bulk string
      * @param input The input stream containing RESP data
      * @return The parsed bulk string
      */
     std::string parseBulkString(std::istringstream& input);
     
     /**
      * @brief Read a line from the input stream
      * @param input The input stream
      * @return The line read from the stream (without CRLF)
      */
     std::string readLine(std::istringstream& input);
 };
 
 #endif // RESP_PROTOCOL_H
 