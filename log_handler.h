#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H

#include <bits/stdc++.h>
#include <ctime>
#include <iomanip>

using namespace std;

// Basic logging function for general entries
inline void logger1(string user_entry)
{
    ofstream logfile("log.txt", ios::app);

    if (!logfile.is_open())
    {
        cerr << "Error: Could not open log file\n";
        return;
    }

    time_t now = time(0);
    tm *ltm = localtime(&now);

    logfile << "["
            << 1900 + ltm->tm_year << "-"
            << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
            << setw(2) << setfill('0') << ltm->tm_mday << " "
            << setw(2) << setfill('0') << ltm->tm_hour << ":"
            << setw(2) << setfill('0') << ltm->tm_min << ":"
            << setw(2) << setfill('0') << ltm->tm_sec
            << "] ";

    logfile << user_entry << endl;

    logfile.close();
}

// Comprehensive operation logging with address, instruction, input/output values, and metadata
// Parameters:
//   operation_type: Type of operation (e.g., "FETCH", "DECODE", "EXECUTE")
//   address: Memory address involved in the operation
//   instruction: Instruction being executed
//   input_value: Optional input value (as vector<bool>)
//   output_value: Optional output value (as vector<bool>)
//   additional_info: Optional additional information
inline void log_operation(string operation_type, unsigned int address, string instruction,
                          vector<bool> input_value = {}, vector<bool> output_value = {},
                          string additional_info = "")
{
    ofstream logfile("log.txt", ios::app);

    if (!logfile.is_open())
    {
        cerr << "Error: Could not open log file for operation logging\n";
        return;
    }

    time_t now = time(0);
    tm *ltm = localtime(&now);

    // Format: [YYYY-MM-DD HH:MM:SS]
    logfile << "\n[" << 1900 + ltm->tm_year << "-"
            << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
            << setw(2) << setfill('0') << ltm->tm_mday << " "
            << setw(2) << setfill('0') << ltm->tm_hour << ":"
            << setw(2) << setfill('0') << ltm->tm_min << ":"
            << setw(2) << setfill('0') << ltm->tm_sec << "] ";

    // Operation Type
    logfile << "OPERATION: " << operation_type << " | ";

    // Address
    logfile << "ADDRESS: 0x" << hex << setw(4) << setfill('0') << address << " | ";

    // Instruction
    logfile << "INSTRUCTION: " << instruction << " | ";

    // Input Value
    if (!input_value.empty())
    {
        logfile << "INPUT: ";
        for (bool bit : input_value)
            logfile << (bit ? "1" : "0");
        logfile << " | ";
    }

    // Output Value
    if (!output_value.empty())
    {
        logfile << "OUTPUT: ";
        for (bool bit : output_value)
            logfile << (bit ? "1" : "0");
        logfile << " | ";
    }

    // Additional Information
    if (!additional_info.empty())
    {
        logfile << "INFO: " << additional_info << " | ";
    }

    logfile << dec << endl;
    logfile.close();
}

// Specialized ALU operation logging
// Parameters:
//   operation_name: Name of ALU operation (e.g., "ADD", "SUB", "MUL", "DIV")
//   address: Memory address
//   instruction: Instruction being executed
//   input1: First operand value
//   input2: Second operand value
//   result: Result of the operation
inline void log_alu_operation(string operation_name, unsigned int address, string instruction,
                              vector<bool> input1 = {}, vector<bool> input2 = {},
                              vector<bool> result = {})
{
    ofstream logfile("log.txt", ios::app);

    if (!logfile.is_open())
    {
        cerr << "Error: Could not open log file for ALU operation logging\n";
        return;
    }

    time_t now = time(0);
    tm *ltm = localtime(&now);

    logfile << "\n[" << 1900 + ltm->tm_year << "-"
            << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
            << setw(2) << setfill('0') << ltm->tm_mday << " "
            << setw(2) << setfill('0') << ltm->tm_hour << ":"
            << setw(2) << setfill('0') << ltm->tm_min << ":"
            << setw(2) << setfill('0') << ltm->tm_sec << "] ";

    logfile << "ALU_OP: " << operation_name << " | ";
    logfile << "ADDR: 0x" << hex << setw(4) << setfill('0') << address << dec << " | ";
    logfile << "INST: " << instruction << " | ";

    if (!input1.empty())
    {
        logfile << "VAL1: ";
        for (bool bit : input1)
            logfile << (bit ? "1" : "0");
        logfile << " | ";
    }

    if (!input2.empty())
    {
        logfile << "VAL2: ";
        for (bool bit : input2)
            logfile << (bit ? "1" : "0");
        logfile << " | ";
    }

    if (!result.empty())
    {
        logfile << "RESULT: ";
        for (bool bit : result)
            logfile << (bit ? "1" : "0");
        logfile << " | ";
    }

    logfile << endl;
    logfile.close();
}
