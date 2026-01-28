#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <cstdint>

// Performance Counters
extern uint64_t cycle_count;        // Total cycles
extern uint64_t instruction_count;  // Instructions completed
extern uint64_t stall_count;        // Stall cycles
extern uint64_t flush_count;        // Flush operations
extern uint64_t forwarding_count;   // Forwarding operations (Assignment IV Part A)

// Initialize performance counters
void initialize_performance();

// Increment cycle counter
void increment_cycle();

// Increment instruction counter
void increment_instruction();

// Increment stall counter
void increment_stall();

// Increment flush counter
void increment_flush();

// Increment forwarding counter (Assignment IV Part A)
void increment_forwarding();

// Calculate and return CPI
double calculate_cpi();

// Display performance statistics
void display_performance();

#endif // PERFORMANCE_H
