#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H

#include <string>
#include <vector>

// Simple logging function
void logger1(std::string user_entry);

// Detailed operation logging
void log_operation(std::string operation_type, unsigned int address, std::string instruction,
                   std::vector<bool> input_value = {}, std::vector<bool> output_value = {},
                   std::string additional_info = "");

// ALU operation logging
void log_alu_operation(std::string operation_name, unsigned int address, std::string instruction,
                       std::vector<bool> input1 = {}, std::vector<bool> input2 = {},
                       std::vector<bool> result = {});

#endif // LOG_HANDLER_H
