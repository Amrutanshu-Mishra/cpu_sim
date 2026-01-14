#ifndef MEMORY_H
#define MEMORY_H

#include <bits/stdc++.h>
using namespace std;

struct memoryElement
{
    unsigned int address; // Memory address
    string instruction;   // Instruction (opcode + operands)
    bool operand[4];      // 4-bit operand
    string mnemonic;      // Assembly mnemonic (ADD, SUB, MUL, etc.)
    unsigned char opcode; // Opcode byte
    string data;          // Data value
    bool valid;           // Flag indicating if memory location is valid
};

// Main memory array - simulating 256 memory locations
extern memoryElement main_memory[256];

// Initialize memory with sample program and data
void initialize_memory();

// Read memory at address
memoryElement read_memory(unsigned int address);

// Write memory at address
void write_memory(unsigned int address, memoryElement element);

// Display memory contents
void display_memory(unsigned int start_address = 0, unsigned int end_address = 255);

// Display program section
void display_program_section();

// Display data section
void display_data_section();

#endif // MEMORY_H
