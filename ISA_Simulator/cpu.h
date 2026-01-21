#ifndef CPU_H
#define CPU_H

#include "log_handler.h"
#include "memory.h"
#include "alu.h"
#include <bits/stdc++.h>
#include <iomanip>
using namespace std;

// CPU Registers (4-bit)
vector<bool> R1(4, 0);  // Register 1
vector<bool> R2(4, 0);  // Register 2
vector<bool> R3(4, 0);  // Register 3
vector<bool> R4(4, 0);  // Register 4
vector<bool> ACC(4, 0); // Accumulator

// Program Counter and Instruction Register
unsigned int PC = 0x00; // Program Counter
string IR = "";         // Instruction Register
unsigned int current_address = 0x00;

// CPU State flags
bool halt_flag = false;
bool overflow_flag = false;
bool zero_flag = false;  // ALU zero flag
bool carry_flag = false; // ALU carry flag

// Function to convert hex string to vector<bool>
inline vector<bool> hex_to_bits(string hex_value)
{
    vector<bool> bits(4, 0);
    if (hex_value.substr(0, 2) == "0x" || hex_value.substr(0, 2) == "0X")
    {
        hex_value = hex_value.substr(2);
    }
    unsigned int value = stoi(hex_value, nullptr, 16);
    for (int i = 3; i >= 0; i--)
    {
        bits[i] = (value >> (3 - i)) & 1;
    }
    return bits;
}

// Function to convert vector<bool> to hex string
inline string bits_to_hex(vector<bool> &bits)
{
    unsigned int value = 0;
    for (int i = 0; i < 4; i++)
    {
        value = (value << 1) | (bits[i] ? 1 : 0);
    }
    stringstream ss;
    ss << "0x" << hex << setw(1) << value;
    return ss.str();
}

// Function to convert vector<bool> to decimal
inline int bits_to_decimal(vector<bool> &bits)
{
    int value = 0;
    for (int i = 0; i < 4; i++)
    {
        value = (value << 1) | (bits[i] ? 1 : 0);
    }
    return value;
}

// LOAD instruction: Load value from memory to register
inline void LOAD_instruction(unsigned int address, string register_name, string hex_value)
{
    memoryElement mem_element = read_memory(address);
    vector<bool> data = hex_to_bits(hex_value);

    cout << "\n>>> Executing LOAD instruction" << endl;
    cout << "    Loading " << hex_value << " into " << register_name << endl;

    if (register_name == "R1")
        R1 = data;
    else if (register_name == "R2")
        R2 = data;
    else if (register_name == "R3")
        R3 = data;
    else if (register_name == "R4")
        R4 = data;
    else if (register_name == "ACC")
        ACC = data;

    log_alu_operation("LOAD", address, "LOAD " + register_name + ", " + hex_value,
                      hex_to_bits(hex_value), {}, data);

    cout << "    Register " << register_name << " = " << bits_to_hex(data)
         << " (decimal: " << bits_to_decimal(data) << ")" << endl;
    PC++;
}

// STORE instruction: Store register value to memory
inline void STORE_instruction(unsigned int address, string register_name)
{
    vector<bool> reg_value(4, 0);

    if (register_name == "R1")
        reg_value = R1;
    else if (register_name == "R2")
        reg_value = R2;
    else if (register_name == "R3")
        reg_value = R3;
    else if (register_name == "R4")
        reg_value = R4;
    else if (register_name == "ACC")
        reg_value = ACC;

    cout << "\n>>> Executing STORE instruction" << endl;
    cout << "    Storing " << bits_to_hex(reg_value) << " from " << register_name
         << " to address 0x" << hex << setw(2) << setfill('0') << address << endl;

    memoryElement mem_element = read_memory(address);
    mem_element.data = bits_to_hex(reg_value);
    write_memory(address, mem_element);

    log_alu_operation("STORE", address, "STORE " + register_name + ", 0x" + to_string(address),
                      reg_value, {}, reg_value);

    PC++;
}

// MOV instruction: Move value from one register to another
inline void MOV_instruction(string dest_register, string src_register)
{
    vector<bool> src_value(4, 0);

    if (src_register == "R1")
        src_value = R1;
    else if (src_register == "R2")
        src_value = R2;
    else if (src_register == "R3")
        src_value = R3;
    else if (src_register == "R4")
        src_value = R4;

    cout << "\n>>> Executing MOV instruction" << endl;
    cout << "    Moving " << bits_to_hex(src_value) << " from " << src_register
         << " to " << dest_register << endl;

    if (dest_register == "R1")
        R1 = src_value;
    else if (dest_register == "R2")
        R2 = src_value;
    else if (dest_register == "R3")
        R3 = src_value;
    else if (dest_register == "R4")
        R4 = src_value;
    else if (dest_register == "ACC")
        ACC = src_value;

    log_alu_operation("MOV", PC, "MOV " + dest_register + ", " + src_register,
                      src_value, {}, src_value);

    PC++;
}

// ADD instruction
inline void ADD_instruction(unsigned int address, string dest_reg, string op1_reg, string op2_reg)
{
    vector<bool> op1(4, 0), op2(4, 0);

    if (op1_reg == "R1")
        op1 = R1;
    else if (op1_reg == "R2")
        op1 = R2;
    else if (op1_reg == "R3")
        op1 = R3;
    else if (op1_reg == "R4")
        op1 = R4;

    if (op2_reg == "R1")
        op2 = R1;
    else if (op2_reg == "R2")
        op2 = R2;
    else if (op2_reg == "R3")
        op2 = R3;
    else if (op2_reg == "R4")
        op2 = R4;

    vector<bool> result = ADD(op1, op2);

    cout << "\n[ADD] " << op1_reg << " (" << bits_to_decimal(op1) << ") + "
         << op2_reg << " (" << bits_to_decimal(op2) << ") = "
         << bits_to_decimal(result) << endl;

    if (dest_reg == "R1")
        R1 = result;
    else if (dest_reg == "R2")
        R2 = result;
    else if (dest_reg == "R3")
        R3 = result;
    else if (dest_reg == "R4")
        R4 = result;
    else if (dest_reg == "ACC")
        ACC = result;

    cout << "    Result stored in " << dest_reg << " = " << bits_to_hex(result)
         << " (decimal: " << bits_to_decimal(result) << ")" << endl;

    log_alu_operation("ADD", address, "ADD " + dest_reg + ", " + op1_reg + ", " + op2_reg,
                      op1, op2, result);

    PC++;
}

// SUB instruction
inline void SUB_instruction(unsigned int address, string dest_reg, string op1_reg, string op2_reg)
{
    vector<bool> op1(4, 0), op2(4, 0);

    if (op1_reg == "R1")
        op1 = R1;
    else if (op1_reg == "R2")
        op1 = R2;
    else if (op1_reg == "R3")
        op1 = R3;
    else if (op1_reg == "R4")
        op1 = R4;

    if (op2_reg == "R1")
        op2 = R1;
    else if (op2_reg == "R2")
        op2 = R2;
    else if (op2_reg == "R3")
        op2 = R3;
    else if (op2_reg == "R4")
        op2 = R4;

    vector<bool> result = SUB(op1, op2);

    cout << "\n[SUB] " << op1_reg << " (" << bits_to_decimal(op1) << ") - "
         << op2_reg << " (" << bits_to_decimal(op2) << ") = "
         << bits_to_decimal(result) << endl;

    if (dest_reg == "R1")
        R1 = result;
    else if (dest_reg == "R2")
        R2 = result;
    else if (dest_reg == "R3")
        R3 = result;
    else if (dest_reg == "R4")
        R4 = result;
    else if (dest_reg == "ACC")
        ACC = result;

    cout << "    Result stored in " << dest_reg << " = " << bits_to_hex(result)
         << " (decimal: " << bits_to_decimal(result) << ")" << endl;

    log_alu_operation("SUB", address, "SUB " + dest_reg + ", " + op1_reg + ", " + op2_reg,
                      op1, op2, result);

    PC++;
}

// Display CPU state
inline void display_cpu_state()
{
    cout << "\n"
         << string(60, '=') << endl;
    cout << "                    CPU STATE" << endl;
    cout << string(60, '=') << endl;
    cout << "Program Counter (PC):     0x" << hex << setw(2) << setfill('0') << PC << dec << endl;
    cout << "Instruction Register:     " << IR << endl;
    cout << "─────────────────────────────────────────────────────────" << endl;
    cout << "R1  (Register 1):         " << bits_to_hex(R1) << "  (decimal: "
         << setw(3) << bits_to_decimal(R1) << ")" << endl;
    cout << "R2  (Register 2):         " << bits_to_hex(R2) << "  (decimal: "
         << setw(3) << bits_to_decimal(R2) << ")" << endl;
    cout << "R3  (Register 3):         " << bits_to_hex(R3) << "  (decimal: "
         << setw(3) << bits_to_decimal(R3) << ")" << endl;
    cout << "R4  (Register 4):         " << bits_to_hex(R4) << "  (decimal: "
         << setw(3) << bits_to_decimal(R4) << ")" << endl;
    cout << "ACC (Accumulator):        " << bits_to_hex(ACC) << "  (decimal: "
         << setw(3) << bits_to_decimal(ACC) << ")" << endl;
    cout << "─────────────────────────────────────────────────────────" << endl;
    cout << "Zero Flag:                " << (zero_flag ? "SET" : "CLEAR") << endl;
    cout << "Carry Flag:               " << (carry_flag ? "SET" : "CLEAR") << endl;
    cout << string(60, '=') << endl
         << endl;
}

// Initialize CPU with sample LOAD instructions
inline void initialize_cpu()
{
    cout << "=== CPU SIMULATOR INITIALIZED ===" << endl;
    cout << "Starting with LOAD instructions..." << endl;

    current_address = 0x00;
    LOAD_instruction(current_address, "R1", "0x05");
    LOAD_instruction(0x01, "R2", "0x03");
    LOAD_instruction(0x02, "R3", "0x0A");
    LOAD_instruction(0x03, "R4", "0x02");

    display_cpu_state();

    logger1("CPU initialized with LOAD instructions");
}

#endif // CPU_H
