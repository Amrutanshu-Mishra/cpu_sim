#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdio>
#include "data_memory.h"
#include "registers.h"
#include "pipeline.h"
#include "performance.h"
#include "log_handler.h"
#include "cache.h"

using namespace std;

// Decoded instruction structure (from pipeline.cpp)
struct DecodedInstruction
{
    uint8_t opcode;
    uint8_t operand;
    uint8_t address_data;
    string mnemonic;
};

// Forward declarations for memory functions
void initialize_memory();
void display_program_section();
DecodedInstruction decode_instruction(uint8_t pc_value);

// External pipeline functions
extern void update_pipeline_register();

// Configuration modes
enum SimMode {
    MODE_NO_OPTIMIZATION = 1,    // Stall-only (Assignment III baseline)
    MODE_FORWARDING_ONLY = 2,    // Forwarding enabled, No cache
    MODE_FORWARDING_CACHE = 3,   // Forwarding + Cache (Full Assignment IV)
    MODE_COMPARISON = 4          // Run all three and compare
};

// Structure to store results for comparison
struct SimulationResult {
    string config_name;
    uint64_t cycles;
    uint64_t instructions;
    double cpi;
    uint64_t stalls;
    uint64_t forwardings;
    uint64_t cache_hits;
    uint64_t cache_misses;
};

// Global configuration
bool forwarding_enabled_global = true;
bool cache_enabled_global = true;

// Print results in exact format required by assignment
void print_results()
{
    printf("\nProgram finished.\n");
    printf("Cycles = %llu\n", (unsigned long long)cycle_count);
    printf("Instructions = %llu\n", (unsigned long long)instruction_count);
    printf("CPI = %.2f\n", calculate_cpi());
    printf("Stalls = %llu\n", (unsigned long long)(stall_count + cache_stall_cycles));
    printf("Forwardings = %llu\n", (unsigned long long)forwarding_count);
    printf("Cache hits = %llu\n", (unsigned long long)cache_hits);
    printf("Cache misses = %llu\n", (unsigned long long)cache_misses);
}

// Run a single simulation with current configuration
SimulationResult run_simulation(bool use_forwarding, bool use_cache, bool verbose)
{
    SimulationResult result;
    
    // Set configuration
    forwarding_enabled_global = use_forwarding;
    cache_enabled_global = use_cache;
    
    // Set config name
    if (!use_forwarding && !use_cache) {
        result.config_name = "No optimization";
    } else if (use_forwarding && !use_cache) {
        result.config_name = "With Forwarding only";
    } else {
        result.config_name = "With Fwd + Cache";
    }
    
    if (verbose) {
        cout << "\n========================================" << endl;
        cout << "  Running: " << result.config_name << endl;
        cout << "========================================" << endl;
    }
    
    // Initialize all components
    initialize_data_memory();
    initialize_registers();
    initialize_memory();
    initialize_pipeline();
    initialize_performance();
    initialize_cache();
    
    // Configure forwarding unit
    forwarding_unit.forward_enabled = use_forwarding;
    
    if (verbose) {
        cout << "  Forwarding: " << (use_forwarding ? "ENABLED" : "DISABLED") << endl;
        cout << "  Cache: " << (use_cache ? "ENABLED" : "DISABLED (direct memory)") << endl;
        cout << endl;
    }
    
    // Main simulation loop
    int max_cycles = 100;
    int cycle = 1;
    
    while (!halt_flag && cycle <= max_cycles)
    {
        if (verbose) {
            cout << "--- CYCLE " << setw(3) << cycle << " ---" << endl;
        }
        
        // Increment cycle counter
        increment_cycle();
        
        // Check for cache stall remaining (only if cache enabled)
        if (use_cache && cache_stall_remaining > 0)
        {
            if (verbose) {
                cout << "  [CACHE] Stalling: " << cache_stall_remaining << " cycles remaining" << endl;
            }
            cache_stall_remaining--;
            cycle++;
            continue;
        }
        
        // Execute current EX stage instruction
        if (ifex_reg.valid)
        {
            if (verbose) {
                cout << "  [EX] Executing: " << ifex_reg.mnemonic << endl;
            }
            
            uint8_t opcode = ifex_reg.opcode;
            uint8_t reg = ifex_reg.operand;
            uint8_t data = ifex_reg.address_data;
            
            // Reset result fields
            ifex_reg.produces_result = false;
            ifex_reg.result_ready = false;
            
            switch (opcode)
            {
                case 0x01: // ADD
                {
                    uint8_t val1 = read_register(reg);
                    uint8_t val2 = read_register((reg + 1) % 16);
                    uint8_t res = val1 + val2;
                    write_register(reg, res);
                    ifex_reg.produces_result = true;
                    ifex_reg.result_value = res;
                    ifex_reg.result_ready = true;
                    ifex_reg.dest_reg = reg;
                    increment_instruction();
                    break;
                }
                case 0x02: // SUB
                {
                    uint8_t val1 = read_register(reg);
                    uint8_t val2 = read_register((reg + 1) % 16);
                    uint8_t res = val1 - val2;
                    write_register(reg, res);
                    ifex_reg.produces_result = true;
                    ifex_reg.result_value = res;
                    ifex_reg.result_ready = true;
                    ifex_reg.dest_reg = reg;
                    increment_instruction();
                    break;
                }
                case 0x03: // MUL
                {
                    uint8_t val1 = read_register(reg);
                    uint8_t val2 = read_register((reg + 1) % 16);
                    uint8_t res = val1 * val2;
                    write_register(reg, res);
                    ifex_reg.produces_result = true;
                    ifex_reg.result_value = res;
                    ifex_reg.result_ready = true;
                    ifex_reg.dest_reg = reg;
                    increment_instruction();
                    break;
                }
                case 0x04: // DIV
                {
                    uint8_t val1 = read_register(reg);
                    uint8_t val2 = read_register((reg + 1) % 16);
                    if (val2 != 0) {
                        uint8_t res = val1 / val2;
                        write_register(reg, res);
                        ifex_reg.produces_result = true;
                        ifex_reg.result_value = res;
                        ifex_reg.result_ready = true;
                        ifex_reg.dest_reg = reg;
                    }
                    increment_instruction();
                    break;
                }
                case 0x0D: // LD
                {
                    MAR = data;
                    if (use_cache) {
                        bool hit;
                        int stall_cycles;
                        MDR = cache_read(MAR, hit, stall_cycles);
                        if (!hit && stall_cycles > 0) {
                            cache_stall_remaining = stall_cycles;
                        }
                    } else {
                        // Direct memory access (no cache)
                        MDR = read_data_memory(MAR);
                    }
                    write_register(reg, MDR);
                    ifex_reg.produces_result = true;
                    ifex_reg.result_value = MDR;
                    ifex_reg.result_ready = true;
                    ifex_reg.dest_reg = reg;
                    increment_instruction();
                    break;
                }
                case 0x0E: // ST
                {
                    MAR = data;
                    MDR = read_register(reg);
                    if (use_cache) {
                        int stall_cycles = cache_write(MAR, MDR);
                        if (stall_cycles > 0) {
                            cache_stall_remaining = stall_cycles;
                        }
                    } else {
                        write_data_memory(MAR, MDR);
                    }
                    increment_instruction();
                    break;
                }
                case 0x08: // JMP
                case 0x0A: // JMP (alternate opcode)
                {
                    PC = data;
                    flush_flag = true;
                    increment_instruction();
                    break;
                }
                case 0x0F: // HALT
                case 0x10:
                {
                    halt_flag = true;
                    increment_instruction();
                    break;
                }
                default:
                    increment_instruction();
                    break;
            }
        }
        
        // Check if cache caused a stall
        if (use_cache && cache_stall_remaining > 0)
        {
            cycle++;
            continue;
        }
        
        // Check for hazards (detect_hazard_or_forward from professor's code)
        bool need_stall = false;
        if (ifex_reg.valid && ifex_reg.is_load && PC < 256)
        {
            DecodedInstruction if_inst = decode_instruction(PC);
            if (if_inst.operand == ifex_reg.dest_reg)
            {
                if (use_forwarding && ifex_reg.result_ready)
                {
                    // Forwarding available - no stall
                    if (verbose) {
                        cout << "  [FORWARDING] R" << (int)ifex_reg.dest_reg 
                             << " forwarded (hazard avoided)" << endl;
                    }
                    increment_forwarding();
                }
                else
                {
                    // No forwarding - must stall
                    if (verbose) {
                        cout << "  [HAZARD] Load-Use detected - STALL" << endl;
                    }
                    need_stall = true;
                    increment_stall();
                }
            }
        }
        
        if (need_stall)
        {
            ifex_reg.valid = false;
            ifex_reg.mnemonic = "BUBBLE";
            cycle++;
            continue;
        }
        
        // Update pipeline register
        if (!halt_flag)
        {
            update_pipeline_register();
        }
        
        // Instruction Fetch (fetch_stage from professor's code)
        if (!halt_flag && !stall_flag)
        {
            PC++;
        }
        
        stall_flag = false;
        flush_flag = false;
        
        cycle++;
    }
    
    // Store results
    result.cycles = cycle_count;
    result.instructions = instruction_count;
    result.cpi = calculate_cpi();
    result.stalls = stall_count + cache_stall_cycles;
    result.forwardings = forwarding_count;
    result.cache_hits = cache_hits;
    result.cache_misses = cache_misses;
    
    if (verbose) {
        print_results();
    }
    
    return result;
}

// Display comparison table
void display_comparison_table(SimulationResult results[3])
{
    cout << "\n" << endl;
    cout << "=================================================================" << endl;
    cout << "          PERFORMANCE COMPARISON TABLE (Assignment IV)" << endl;
    cout << "=================================================================" << endl;
    cout << endl;
    
    // Simple text-based table
    cout << "+--------------------------+--------+-------+--------+-------------+" << endl;
    cout << "| Version                  | Cycles |  CPI  | Stalls | Cache Misses|" << endl;
    cout << "+--------------------------+--------+-------+--------+-------------+" << endl;
    
    // Row 1: No optimization
    cout << "| No optimization          |   " << setfill(' ') << setw(4) << results[0].cycles 
         << " | " << fixed << setprecision(2) << results[0].cpi 
         << " |   " << setw(4) << results[0].stalls 
         << " |     N/A     |" << endl;
    
    // Row 2: Forwarding only
    cout << "| With Forwarding only     |   " << setfill(' ') << setw(4) << results[1].cycles 
         << " | " << fixed << setprecision(2) << results[1].cpi 
         << " |   " << setw(4) << results[1].stalls 
         << " |     N/A     |" << endl;
    
    // Row 3: Forwarding + Cache
    cout << "| With Fwd + Cache         |   " << setfill(' ') << setw(4) << results[2].cycles 
         << " | " << fixed << setprecision(2) << results[2].cpi 
         << " |   " << setw(4) << results[2].stalls 
         << " |      " << setw(4) << results[2].cache_misses << "     |" << endl;
    
    cout << "+--------------------------+--------+-------+--------+-------------+" << endl;
    cout << endl;
    
    // Detailed metrics in assignment format
    cout << "Detailed Metrics (Assignment IV Format):" << endl;
    cout << "-----------------------------------------" << endl;
    
    for (int i = 0; i < 3; i++) {
        cout << "\n" << results[i].config_name << ":" << endl;
        printf("  Cycles = %llu\n", (unsigned long long)results[i].cycles);
        printf("  Instructions = %llu\n", (unsigned long long)results[i].instructions);
        printf("  CPI = %.2f\n", results[i].cpi);
        printf("  Stalls = %llu\n", (unsigned long long)results[i].stalls);
        printf("  Forwardings = %llu\n", (unsigned long long)results[i].forwardings);
        if (i == 2) {
            printf("  Cache hits = %llu\n", (unsigned long long)results[i].cache_hits);
            printf("  Cache misses = %llu\n", (unsigned long long)results[i].cache_misses);
        }
    }
    
    // Analysis
    cout << "\n==================================================================" << endl;
    cout << "ANALYSIS:" << endl;
    cout << "==================================================================" << endl;
    
    // Forwarding benefit
    double forwarding_improvement = 0;
    if (results[0].cpi > 0) {
        forwarding_improvement = ((results[0].cpi - results[1].cpi) / results[0].cpi) * 100;
    }
    cout << "\n1. Forwarding Benefit (Part A):" << endl;
    cout << "   - Stalls reduced from " << results[0].stalls << " to " << results[1].stalls << endl;
    printf("   - CPI improved from %.2f to %.2f\n", results[0].cpi, results[1].cpi);
    printf("   - Performance improvement: %.1f%%\n", forwarding_improvement);
    
    // Cache impact
    cout << "\n2. Cache Impact (Part B):" << endl;
    cout << "   - Cache hits: " << results[2].cache_hits << ", Cache misses: " << results[2].cache_misses << endl;
    double hit_rate = 0;
    if (results[2].cache_hits + results[2].cache_misses > 0) {
        hit_rate = (double)results[2].cache_hits / (results[2].cache_hits + results[2].cache_misses) * 100;
    }
    printf("   - Hit rate: %.1f%%\n", hit_rate);
    cout << "   - Cache stall cycles: " << (results[2].stalls - results[1].stalls) << endl;
    
    cout << "\n==================================================================" << endl;
}

int main(int argc, char* argv[])
{
    cout << "========================================" << endl;
    cout << "  CPU SIMULATOR - Assignment IV" << endl;
    cout << "  Performance Optimization:" << endl;
    cout << "  - Part A: Data Forwarding" << endl;
    cout << "  - Part B: Direct-Mapped Cache" << endl;
    cout << "========================================\n" << endl;
    
    // Parse command line arguments
    SimMode mode = MODE_COMPARISON;  // Default to comparison mode
    
    if (argc > 1) {
        int arg = atoi(argv[1]);
        if (arg >= 1 && arg <= 4) {
            mode = (SimMode)arg;
        }
    }
    
    cout << "Select mode:" << endl;
    cout << "  1 = No optimization (Stall-only, Assignment III baseline)" << endl;
    cout << "  2 = With Forwarding only" << endl;
    cout << "  3 = With Forwarding + Cache" << endl;
    cout << "  4 = Run ALL configurations and compare (default)" << endl;
    cout << "\nRunning mode: " << mode << endl;
    
    if (mode == MODE_COMPARISON)
    {
        cout << "\n*** RUNNING ALL THREE CONFIGURATIONS ***\n" << endl;
        
        // Display program first
        initialize_memory();
        cout << "=== TEST PROGRAM ===" << endl;
        display_program_section();
        
        // Array to store results
        SimulationResult results[3];
        
        // Run configuration 1: No optimization
        cout << "\n[CONFIG 1] Running: No optimization (stall-only)..." << endl;
        results[0] = run_simulation(false, false, false);
        
        // Run configuration 2: Forwarding only
        cout << "[CONFIG 2] Running: With Forwarding only..." << endl;
        results[1] = run_simulation(true, false, false);
        
        // Run configuration 3: Forwarding + Cache
        cout << "[CONFIG 3] Running: With Forwarding + Cache..." << endl;
        results[2] = run_simulation(true, true, false);
        
        // Display comparison table
        display_comparison_table(results);
        
        // Log to file
        logger1("=== COMPARISON RUN (Assignment IV) ===");
        for (int i = 0; i < 3; i++) {
            logger1("Configuration: " + results[i].config_name);
            logger1("  Cycles: " + to_string(results[i].cycles));
            logger1("  Instructions: " + to_string(results[i].instructions));
            logger1("  CPI: " + to_string(results[i].cpi));
            logger1("  Stalls: " + to_string(results[i].stalls));
            logger1("  Forwardings: " + to_string(results[i].forwardings));
        }
        logger1("=== END COMPARISON ===");
    }
    else
    {
        // Run single configuration
        bool use_fwd = (mode == MODE_FORWARDING_ONLY || mode == MODE_FORWARDING_CACHE);
        bool use_cache = (mode == MODE_FORWARDING_CACHE);
        
        // Display program
        initialize_memory();
        cout << "=== TEST PROGRAM ===" << endl;
        display_program_section();
        
        SimulationResult result = run_simulation(use_fwd, use_cache, true);
        
        // Display final state
        cout << "\n========================================" << endl;
        cout << "        EXECUTION COMPLETED" << endl;
        cout << "========================================" << endl;
        
        display_registers();
        
        if (use_cache) {
            display_cache();
            display_cache_stats();
        }
        
        display_performance();
    }
    
    cout << "\n========================================" << endl;
    cout << "        SIMULATION FINISHED" << endl;
    cout << "========================================" << endl;
    
    return 0;
}
