#ifndef DATA_MEMORY_H
#define DATA_MEMORY_H

#include <cstdint>

// Data Memory: 256 locations × 8-bit each
extern uint8_t data_memory[256];

// Initialize data memory (clear to 0)
void initialize_data_memory();

// Read 8-bit value from data memory
uint8_t read_data_memory(uint8_t address);

// Write 8-bit value to data memory
void write_data_memory(uint8_t address, uint8_t value);

// Display data memory contents for debugging
void display_data_memory(uint8_t start_address = 0, uint8_t end_address = 255);

#endif // DATA_MEMORY_H
