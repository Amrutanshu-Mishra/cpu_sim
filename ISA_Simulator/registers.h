#ifndef REGISTERS_H
#define REGISTERS_H

#include <cstdint>
#include <vector>

using namespace std;

// Register File: R0-R15 (16 registers, 8-bit each)
extern uint8_t register_file[16];

// Special Registers
extern uint8_t MAR;  // Memory Address Register
extern uint8_t MDR;  // Memory Data Register
extern uint8_t PC;   // Program Counter
extern bool halt_flag;  // Halt flag

// Initialize all registers to 0
void initialize_registers();

// Read from register file
uint8_t read_register(uint8_t reg_num);

// Write to register file
void write_register(uint8_t reg_num, uint8_t value);

// Display all register contents
void display_registers();

#endif // REGISTERS_H
