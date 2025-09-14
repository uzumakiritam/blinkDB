/**
 * @file repl.cpp
 * @brief REPL (Read-Eval-Print Loop) for BLINK DB storage engine
 * 
 * This file implements a simple command-line interface for interacting
 * with the BLINK DB storage engine.
 */

 #include "storage_engine.h"
 #include <iostream>
 #include <string>
 #include <sstream>
 #include <regex>
 
 /**
  * @brief Parse and execute a command
  * @param engine Reference to the storage engine
  * @param command The command to parse and execute
  */
 void executeCommand(StorageEngine& engine, const std::string& command) {
    
    //  std::regex set_regex(R"(SET\s+(\S+)\s+"([^"]*)")");
    std::regex set_regex("SET\\s+(\\S+)\\s+\"([^\"]*)\"");
     std::regex get_regex(R"(GET\s+(\S+))");
     std::regex del_regex(R"(DEL\s+(\S+))");
     
     std::smatch matches;
     
     if (std::regex_match(command, matches, set_regex)) {
         std::string key = matches[1];
         std::string value = matches[2];
         engine.set(key, value);
     }
     else if (std::regex_match(command, matches, get_regex)) {
         std::string key = matches[1];
         std::string value = engine.get(key);
         std::cout << value << std::endl;
     }
     else if (std::regex_match(command, matches, del_regex)) {
         std::string key = matches[1];
         bool success = engine.del(key);
         if (!success) {
             std::cout << "Does not exist." << std::endl;
         }
     }
     else {
         std::cout << "Invalid command. Supported commands: SET <key> \"<value>\", GET <key>, DEL <key>" << std::endl;
     }
 }
 
 /**
  * @brief Main function for the REPL
  * @return Exit code
  */
 int main() {
     StorageEngine engine;
     std::string command;
     
     while (true) {
         std::cout << "User> ";
         std::getline(std::cin, command);
         
         if (command == "EXIT" || command == "exit") {
             break;
         }
         
         executeCommand(engine, command);
     }
     
     return 0;
 }
 