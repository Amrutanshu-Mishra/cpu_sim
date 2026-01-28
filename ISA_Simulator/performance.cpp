#include "performance.h"
#include "cache.h"
#include <iostream>
#include <iomanip>

using namespace std;

// Performance Counters
uint64_t cycle_count = 0;
uint64_t instruction_count = 0;
uint64_t stall_count = 0;
uint64_t flush_count = 0;
uint64_t forwarding_count = 0;

// Initialize performance counters
void initialize_performance()
{
    cycle_count = 0;
    instruction_count = 0;
    stall_count = 0;
    flush_count = 0;
    forwarding_count = 0;
}

// Increment cycle counter
void increment_cycle()
{
    cycle_count++;
}

// Increment instruction counter
void increment_instruction()
{
    instruction_count++;
}

// Increment stall counter
void increment_stall()
{
    stall_count++;
}

// Increment flush counter
void increment_flush()
{
    flush_count++;
}

// Increment forwarding counter (Assignment IV Part A)
void increment_forwarding()
{
    forwarding_count++;
}

// Calculate CPI (Cycles Per Instruction)
double calculate_cpi()
{
    if (instruction_count == 0)
        return 0.0;
    return (double)cycle_count / (double)instruction_count;
}

// Display performance statistics in assignment-required format
void display_performance()
{
    // Calculate total stalls (hazard + cache)
    uint64_t total_stalls = stall_count + cache_stall_cycles;
    
    cout << "\n========================================" << endl;
    cout << "     PERFORMANCE MEASUREMENT" << endl;
    cout << "========================================" << endl;
    cout << "Total cycles = " << cycle_count << endl;
    cout << "Total instructions = " << instruction_count << endl;
    cout << "CPI = cycles / instructions = " << fixed << setprecision(3) << calculate_cpi() << endl;
    cout << "Number of stalls = " << total_stalls << endl;
    cout << "Number of forwardings = " << forwarding_count << endl;
    cout << "Cache hits = " << cache_hits << endl;
    cout << "Cache misses = " << cache_misses << endl;
    cout << "========================================" << endl;
    cout << endl;
    
    // Detailed breakdown
    cout << "--- Detailed Breakdown ---" << endl;
    cout << "  Hazard stalls:     " << stall_count << endl;
    cout << "  Cache stall cycles: " << cache_stall_cycles << endl;
    cout << "  Flush operations:   " << flush_count << endl;
    
    uint64_t total_accesses = cache_hits + cache_misses;
    double hit_rate = 0.0;
    if (total_accesses > 0)
        hit_rate = (double)cache_hits / (double)total_accesses * 100.0;
    cout << "  Cache hit rate:     " << fixed << setprecision(2) << hit_rate << "%" << endl;
    cout << endl;
}
