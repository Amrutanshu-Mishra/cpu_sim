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

// Function to convert hex string to vector<bool>
vector<bool> hex_to_bits(string hex_value)
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
string bits_to_hex(vector<bool> &bits)
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

// LOAD instruction: Load value from memory to register
void LOAD_instruction(unsigned int address, string register_name, string hex_value)
{
    memoryElement mem_element = read_memory(address);
    vector<bool> data = hex_to_bits(hex_value);

    cout << "\n>>> Executing LOAD instruction" << endl;
    cout << "    Loading 0x" << hex << setfill('0') << setw(2) << hex_value.substr(2)
         << " into " << register_name << endl;

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

    // Log the operation
    log_alu_operation("LOAD", address, "LOAD " + register_name + ", " + hex_value,
                      hex_to_bits(hex_value), {}, data);

    cout << "    Register " << register_name << " = " << bits_to_hex(data) << endl;
    PC++;
}

// STORE instruction: Store register value to memory
void STORE_instruction(unsigned int address, string register_name)
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

    log_alu_operation("STORE", address, "STORE " + register_name + ", 0x" + to_string(address), reg_value, {}, reg_value);

    PC++;
}

// MOV instruction: Move value from one register to another
void MOV_instruction(string dest_register, string src_register)
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

// Display CPU state
void display_cpu_state()
{
    cout << "\n=== CPU STATE ===" << endl;
    cout << "Program Counter (PC):   0x" << hex << setw(2) << setfill('0') << PC << endl;
    cout << "Instruction Register:   " << IR << endl;
    cout << "R1:  " << bits_to_hex(R1) << "  |  ";
    cout << "R2:  " << bits_to_hex(R2) << "  |  ";
    cout << "R3:  " << bits_to_hex(R3) << "  |  ";
    cout << "R4:  " << bits_to_hex(R4) << endl;
    cout << "ACC: " << bits_to_hex(ACC) << endl;
    cout << "Halt Flag: " << (halt_flag ? "TRUE" : "FALSE") << endl;
    cout << "Zero Flag: " << (zero_flag ? "TRUE" : "FALSE") << endl;
    cout << "Carry Flag: " << (carry_flag ? "TRUE" : "FALSE") << endl;
    cout << dec << endl;
}

// Initialize CPU with sample LOAD instructions
void initialize_cpu()
{
    cout << "=== CPU SIMULATOR INITIALIZED ===" << endl;
    cout << "Starting with LOAD instructions..." << endl;

    // LOAD instruction 1: Load 0x42 (66 in decimal) into R1
    current_address = 0x00;
    LOAD_instruction(current_address, "R1", "0x42");

    // LOAD instruction 2: Load 0x15 (21 in decimal) into R2
    current_address = 0x01;
    LOAD_instruction(current_address, "R2", "0x15");

    // LOAD instruction 3: Load 0x78 (120 in decimal) into R3
    current_address = 0x02;
    LOAD_instruction(current_address, "R3", "0x78");

    // LOAD instruction 4: Load 0x2A (42 in decimal) into R4
    current_address = 0x03;
    LOAD_instruction(current_address, "R4", "0x2A");

    // Display CPU state
    display_cpu_state();

    // Log CPU initialization
    logger1("CPU initialized with LOAD instructions");
}

// Main execution function
int main()
{
    initialize_memory();
    initialize_cpu();

    cout << "\n=== PROGRAM EXECUTION ===" << endl;

    // Example: Add R1 and R2
    cout << "\n>>> Adding R1 and R2..." << endl;
    vector<bool> result = ADD(R1, R2);
    ACC = result;
    cout << "Result: " << bits_to_hex(R1) << " + " << bits_to_hex(R2)
         << " = " << bits_to_hex(result) << endl;
    log_alu_operation("ADD", PC, "ADD R1, R2", R1, R2, result);

    // Display final CPU state
    display_cpu_state();

    // Display memory sections
    display_program_section();
    display_data_section();

    logger1("CPU execution completed");

    return 0;
}
