/**
 * @file resp_protocol.cpp
 * @brief Implementation of RESP (Redis Serialization Protocol)
 * @author Your Name
 * @date March 31, 2025
 */

 #include "resp_protocol.h"
 #include <iostream>
 
 /**
  * @brief Parse RESP data from a buffer
  * @param buffer The buffer containing RESP data
  * @return A vector of strings representing the parsed command
  * 
  * This function takes a buffer containing RESP-formatted data and parses it
  * into a vector of strings representing the command and its arguments.
  */
 std::vector<std::string> RespProtocol::parseRequest(const std::string& buffer) {
     std::istringstream input(buffer);
     std::string line = readLine(input);
     
     if (line.empty()) {
         return {};
     }
     
     // Check if it's an array
     if (line[0] == '*') {
         int count = std::stoi(line.substr(1));
         return parseArray(input, count);
     }
     
     // If not an array, return empty result
     return {};
 }
 
 /**
  * @brief Parse a RESP array
  * @param input The input stream containing RESP data
  * @param count The number of elements in the array
  * @return A vector of strings representing the parsed array
  * 
  * This function parses a RESP array with the specified number of elements.
  */
 std::vector<std::string> RespProtocol::parseArray(std::istringstream& input, int count) {
     std::vector<std::string> result;
     
     for (int i = 0; i < count; i++) {
         std::string line = readLine(input);
         
         if (line.empty()) {
             break;
         }
         
         if (line[0] == '$') {
             // Bulk string
             result.push_back(parseBulkString(input));
         }
         // Add support for other types if needed
     }
     
     return result;
 }
 
 /**
  * @brief Parse a RESP bulk string
  * @param input The input stream containing RESP data
  * @return The parsed bulk string
  * 
  * This function parses a RESP bulk string from the input stream.
  */
 std::string RespProtocol::parseBulkString(std::istringstream& input) {
     return readLine(input);
 }
 
 /**
  * @brief Read a line from the input stream
  * @param input The input stream
  * @return The line read from the stream (without CRLF)
  * 
  * This function reads a line from the input stream until a CR character,
  * then consumes the following LF character.
  */
 std::string RespProtocol::readLine(std::istringstream& input) {
     std::string line;
     std::getline(input, line, '\r');
     
     // Consume the \n
     input.get();
     
     return line;
 }
 
 /**
  * @brief Encode a simple string response
  * @param str The string to encode
  * @return RESP-encoded simple string
  * 
  * This function encodes a simple string according to the RESP protocol.
  * Format: +<string>\r\n
  */
 std::string RespProtocol::encodeSimpleString(const std::string& str) {
     return "+" + str + "\r\n";
 }
 
 /**
  * @brief Encode an error response
  * @param err The error message to encode
  * @return RESP-encoded error
  * 
  * This function encodes an error message according to the RESP protocol.
  * Format: -<error message>\r\n
  */
 std::string RespProtocol::encodeError(const std::string& err) {
     return "-" + err + "\r\n";
 }
 
 /**
  * @brief Encode an integer response
  * @param num The integer to encode
  * @return RESP-encoded integer
  * 
  * This function encodes an integer according to the RESP protocol.
  * Format: :<integer>\r\n
  */
 std::string RespProtocol::encodeInteger(int64_t num) {
     return ":" + std::to_string(num) + "\r\n";
 }
 
 /**
  * @brief Encode a bulk string response
  * @param str The string to encode
  * @return RESP-encoded bulk string
  * 
  * This function encodes a bulk string according to the RESP protocol.
  * Format: $<length>\r\n<string>\r\n
  */
 std::string RespProtocol::encodeBulkString(const std::string& str) {
     return "$" + std::to_string(str.length()) + "\r\n" + str + "\r\n";
 }
 
 /**
  * @brief Encode a null bulk string
  * @return RESP-encoded null bulk string
  * 
  * This function encodes a null bulk string according to the RESP protocol.
  * Format: $-1\r\n
  */
 std::string RespProtocol::encodeNull() {
     return "$-1\r\n";
 }
 
 /**
  * @brief Encode an array response
  * @param arr The array of strings to encode
  * @return RESP-encoded array
  * 
  * This function encodes an array of strings according to the RESP protocol.
  * Format: *<count>\r\n<element1>...<elementN>
  */
 std::string RespProtocol::encodeArray(const std::vector<std::string>& arr) {
     std::string result = "*" + std::to_string(arr.size()) + "\r\n";
     
     for (const auto& item : arr) {
         result += encodeBulkString(item);
     }
     
     return result;
 }
 