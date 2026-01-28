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
memoryElement main_memory[256];

// Initialize memory with sample program and data
inline void initialize_memory()
{
    // Clear memory
    for (int i = 0; i < 256; i++)
    {
        main_memory[i].address = i;
        main_memory[i].instruction = "";
        main_memory[i].mnemonic = "";
        main_memory[i].opcode = 0;
        main_memory[i].data = "0x00";
        main_memory[i].valid = false;
        for (int j = 0; j < 4; j++)
            main_memory[i].operand[j] = 0;
    }

    // Program section (addresses 0x00 - 0x3F)
    main_memory[0x00] = {0x00, "ADD R1, R2", {0, 0, 0, 1}, "ADD", 0x01, "0x00", true};
    main_memory[0x01] = {0x01, "SUB R3, R4", {0, 0, 1, 0}, "SUB", 0x02, "0x00", true};
    main_memory[0x02] = {0x02, "MUL R1, R3", {0, 0, 1, 1}, "MUL", 0x03, "0x00", true};
    main_memory[0x03] = {0x03, "DIV R2, R4", {0, 1, 0, 0}, "DIV", 0x04, "0x00", true};
    main_memory[0x04] = {0x04, "MOV R1, 0x05", {0, 1, 0, 1}, "MOV", 0x05, "0x05", true};
    main_memory[0x05] = {0x05, "LOAD R2, 0x80", {0, 1, 1, 0}, "LOAD", 0x06, "0x80", true};
    main_memory[0x06] = {0x06, "STORE R3, 0x81", {0, 1, 1, 1}, "STORE", 0x07, "0x81", true};
    main_memory[0x07] = {0x07, "JMP 0x10", {1, 0, 0, 0}, "JMP", 0x08, "0x10", true};

    // Additional program instructions
    main_memory[0x08] = {0x08, "AND R1, R2", {1, 0, 0, 1}, "AND", 0x09, "0x00", true};
    main_memory[0x09] = {0x09, "OR R3, R4", {1, 0, 1, 0}, "OR", 0x0A, "0x00", true};
    main_memory[0x0A] = {0x0A, "XOR R1, R3", {1, 0, 1, 1}, "XOR", 0x0B, "0x00", true};
    main_memory[0x0B] = {0x0B, "NOT R2", {1, 1, 0, 0}, "NOT", 0x0C, "0x00", true};
    main_memory[0x0C] = {0x0C, "SHL R1, 2", {1, 1, 0, 1}, "SHL", 0x0D, "0x02", true};
    main_memory[0x0D] = {0x0D, "SHR R3, 1", {1, 1, 1, 0}, "SHR", 0x0E, "0x01", true};
    main_memory[0x0E] = {0x0E, "CMP R1, R2", {1, 1, 1, 1}, "CMP", 0x0F, "0x00", true};
    main_memory[0x0F] = {0x0F, "HLT", {0, 0, 0, 0}, "HLT", 0x10, "0x00", true};

    // Data section (addresses 0x80 - 0xFF)
    main_memory[0x80] = {0x80, "DATA: 0x42", {0, 1, 0, 0}, "DATA", 0x00, "0x42", true};
    main_memory[0x81] = {0x81, "DATA: 0x15", {0, 0, 0, 1}, "DATA", 0x00, "0x15", true};
    main_memory[0x82] = {0x82, "DATA: 0x78", {0, 1, 1, 1}, "DATA", 0x00, "0x78", true};
    main_memory[0x83] = {0x83, "DATA: 0x2A", {0, 0, 1, 0}, "DATA", 0x00, "0x2A", true};
    main_memory[0x84] = {0x84, "DATA: 0xFF", {1, 1, 1, 1}, "DATA", 0x00, "0xFF", true};
    main_memory[0x85] = {0x85, "DATA: 0x00", {0, 0, 0, 0}, "DATA", 0x00, "0x00", true};
}

// Read memory at address
inline memoryElement read_memory(unsigned int address)
{
    if (address < 256)
        return main_memory[address];
    else
    {
        cerr << "Memory access out of bounds: " << address << endl;
        return main_memory[0];
    }
}

// Write memory at address
inline void write_memory(unsigned int address, memoryElement element)
{
    if (address < 256)
        main_memory[address] = element;
    else
        cerr << "Memory write out of bounds: " << address << endl;
}

// Display memory contents
inline void display_memory(unsigned int start_address = 0, unsigned int end_address = 255)
{
    cout << "\n=== CPU Memory Contents ===" << endl;
    cout << "Address | Instruction         | Mnemonic | Opcode | Data   | Valid" << endl;
    cout << "--------|----------------------|----------|--------|--------|-------" << endl;

    for (unsigned int i = start_address; i <= end_address && i < 256; i++)
    {
        if (main_memory[i].valid)
        {
            cout << "0x" << hex << setw(2) << setfill('0') << i << "     | "
                 << setfill(' ') << setw(20) << left << main_memory[i].instruction << "| "
                 << setw(8) << left << main_memory[i].mnemonic << "| "
                 << "0x" << hex << setw(2) << setfill('0') << (int)main_memory[i].opcode << "   | "
                 << main_memory[i].data << "    | "
                 << (main_memory[i].valid ? "YES" : "NO") << endl;
        }
    }
    cout << dec << endl;
}

// Display program section
inline void display_program_section()
{
    cout << "\n=== PROGRAM SECTION (0x00 - 0x0F) ===" << endl;
    display_memory(0x00, 0x0F);
}

// Display data section
inline void display_data_section()
{
    cout << "\n=== DATA SECTION (0x80 - 0x8F) ===" << endl;
    display_memory(0x80, 0x8F);
}

#endif // MEMORY_H
