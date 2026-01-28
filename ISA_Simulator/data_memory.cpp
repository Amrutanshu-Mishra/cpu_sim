#include "data_memory.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

// Data Memory: 256 × 8-bit
uint8_t data_memory[256];

// Initialize data memory - clear all to 0
void initialize_data_memory()
{
    memset(data_memory, 0, 256);
    
    // Initialize some sample data for testing
    data_memory[10] = 0x42;  // 66 in decimal
    data_memory[11] = 0x15;  // 21 in decimal
    data_memory[12] = 0x78;  // 120 in decimal
    data_memory[13] = 0x2A;  // 42 in decimal
    data_memory[14] = 0xFF;  // 255 in decimal
    data_memory[15] = 0x00;  // 0 in decimal
}

// Read 8-bit value from data memory
uint8_t read_data_memory(uint8_t address)
{
    return data_memory[address];
}

// Write 8-bit value to data memory
void write_data_memory(uint8_t address, uint8_t value)
{
    data_memory[address] = value;
}

// Display data memory contents
void display_data_memory(uint8_t start_address, uint8_t end_address)
{
    cout << "\n=== DATA MEMORY CONTENTS ===" << endl;
    cout << "Address | Hex Value | Decimal Value" << endl;
    cout << "--------|-----------|---------------" << endl;
    
    for (unsigned int i = start_address; i <= end_address; i++)
    {
        // Only show non-zero values or first 16 locations
        if (data_memory[i] != 0 || i < 16)
        {
            cout << "0x" << hex << setw(2) << setfill('0') << i << "    | "
                 << "0x" << setw(2) << setfill('0') << (int)data_memory[i] << "      | "
                 << dec << setw(3) << (int)data_memory[i] << endl;
        }
    }
    cout << dec << endl;
}
