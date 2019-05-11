#ifndef H_LOGGER
#define H_LOGGER

#include <iostream>
#include <string>
#include <thread>
#include <queue>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

struct Log {
  std::string module_path="", function_name="", line_number="", request_id="", message="";
};

class Logger {
private:
std::thread log_thread;
std::queue<std::pair<int, std::string>> log_queue;
std::string request_id;

void push_request_id(std::string request_id) {
    this->request_id = request_id;
}

void consume_log(std::queue<std::pair<int, std::string>> &logger_queue) {
    while (true) {
        if (logger_queue.empty())
            continue;

        std::pair<int, std::string> log_message = logger_queue.front();
        if (log_message.first == -1)
            break;
        
        if (log_message.first == 0) {
            spdlog::get("console")->info(log_message.second);
        }
        else if (log_message.first == 1) {
            spdlog::get("console")->warn(log_message.second);

        }
        else if (log_message.first == 2) {
            spdlog::get("console")->error(log_message.second);
        }
        logger_queue.pop();
    }
}

public:
    Logger() {
        spdlog::set_pattern("%^%Y-%m-%dT%H:%M:%S.%f%z%$ | %v | process id: %P | thread id: %t");
        auto console = spdlog::stdout_color_mt("console");    
        
        log_thread = std::thread(&Logger::consume_log, this, std::ref(log_queue));
    }

    void error(Log log = [](Log log) mutable { 
                           log.module_path="";
                           log.function_name="";
                           log.line_number="";
                           log.request_id = "";
                           log.message="";
                           
                           return log; }(Log())
            ) {

      std::string module_path = log.module_path;
      std::string function_name = log.function_name; 
      std::string line_number = log.line_number; 
      
      if (log.request_id != "") {
          push_request_id(log.request_id);
      }
      std::string request_id = this->request_id; 
      std::string message = std::move(log.message);
      
      std::string level = "ERROR";
      char buffer[1024 * 16];

      sprintf (buffer, "%s | %s:%s:%s | %s | ", level.c_str(), 
                                                module_path.c_str(), 
                                                function_name.c_str(),
                                                line_number.c_str(), 
                                                request_id.c_str());
      
      std::string string_buffer = buffer + message;

      log_queue.push(std::make_pair(2, string_buffer));   
    }

    void warn(Log log = [](Log log) mutable { 
                           log.module_path="";
                           log.function_name="";
                           log.line_number="";
                           log.request_id = "";
                           log.message="";
                           
                           return log; }(Log())
            ) {

      std::string module_path = log.module_path;
      std::string function_name = log.function_name; 
      std::string line_number = log.line_number; 

      if (log.request_id != "") {
          push_request_id(log.request_id);
      }
      std::string request_id = this->request_id;
      std::string message = std::move(log.message);
      
      std::string level = "WARN";
      char buffer[1024 * 16];
      
      sprintf (buffer, "%s | %s:%s:%s | %s | ", level.c_str(), 
                                                module_path.c_str(), 
                                                function_name.c_str(),
                                                line_number.c_str(), 
                                                request_id.c_str());
      
      std::string string_buffer = buffer + message;

      log_queue.push(std::make_pair(1, string_buffer));
    }

    void info(Log log = [](Log log) mutable { 
                           log.module_path="";
                           log.function_name="";
                           log.line_number="";
                           log.request_id = "";
                           log.message="";
                           
                           return log; }(Log())
            ) {

      std::string module_path = log.module_path;
      std::string function_name = log.function_name; 
      std::string line_number = log.line_number; 

      if (log.request_id != "") {
          push_request_id(log.request_id);
      }
      std::string request_id = this->request_id;
      std::string message = log.message;
      
      std::string level = "INFO";
      char buffer[1024 * 16];
      
      sprintf (buffer, "%s | %s:%s:%s | %s | ", level.c_str(), 
                                                module_path.c_str(), 
                                                function_name.c_str(),
                                                line_number.c_str(), 
                                                request_id.c_str());
      
      std::string string_buffer = buffer + message;
    
      log_queue.push(std::make_pair(0, string_buffer));                            
    }

    std::string get_request_id() {
        return this->request_id;
    }

    ~Logger() {
        log_queue.push(std::make_pair(-1, ""));   
    }
  };  

#endif  // H_PRETTY_PRINT


