#include "registers.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

// Register File: 16 registers × 8-bit
uint8_t register_file[16];

// Special Registers
uint8_t MAR = 0;   // Memory Address Register
uint8_t MDR = 0;   // Memory Data Register
uint8_t PC = 0;    // Program Counter
bool halt_flag = false;

// Initialize all registers to 0
void initialize_registers()
{
    memset(register_file, 0, 16);
    MAR = 0;
    MDR = 0;
    PC = 0;
    halt_flag = false;
    
    // Initialize some registers with test values
    register_file[0] = 0x00;  // R0 always 0 (common convention)
    register_file[1] = 0x05;  // R1 = 5
    register_file[2] = 0x03;  // R2 = 3
    register_file[3] = 0x08;  // R3 = 8
    register_file[4] = 0x02;  // R4 = 2
}

// Read from register file
uint8_t read_register(uint8_t reg_num)
{
    if (reg_num < 16)
        return register_file[reg_num];
    else
    {
        cerr << "Invalid register number: " << (int)reg_num << endl;
        return 0;
    }
}

// Write to register file
void write_register(uint8_t reg_num, uint8_t value)
{
    if (reg_num < 16)
    {
        if (reg_num == 0)
        {
            // R0 is typically hardwired to 0
            cout << "Warning: Attempted write to R0 (hardwired to 0)" << endl;
            register_file[0] = 0;
        }
        else
        {
            register_file[reg_num] = value;
        }
    }
    else
    {
        cerr << "Invalid register number: " << (int)reg_num << endl;
    }
}

// Display all register contents
void display_registers()
{
    cout << "\n=== CPU REGISTERS ===" << endl;
    cout << "Register | Hex  | Decimal" << endl;
    cout << "---------|------|--------" << endl;
    
    for (int i = 0; i < 16; i++)
    {
        cout << "R" << dec << setw(2) << i << "      | "
             << "0x" << hex << setw(2) << setfill('0') << (int)register_file[i] << " | "
             << dec << setw(3) << (int)register_file[i] << endl;
    }
    
    cout << "\n=== SPECIAL REGISTERS ===" << endl;
    cout << "PC  = 0x" << hex << setw(2) << setfill('0') << (int)PC << " (" << dec << (int)PC << ")" << endl;
    cout << "MAR = 0x" << hex << setw(2) << setfill('0') << (int)MAR << " (" << dec << (int)MAR << ")" << endl;
    cout << "MDR = 0x" << hex << setw(2) << setfill('0') << (int)MDR << " (" << dec << (int)MDR << ")" << endl;
    cout << "HALT = " << (halt_flag ? "TRUE" : "FALSE") << endl;
    cout << dec << endl;
}
