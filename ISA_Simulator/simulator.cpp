#include <bits/stdc++.h>
#include <iomanip>
#include "alu.h"
#include "memory.h"
#include "log_handler.h"
#include "cpu.h"

using namespace std;

// ============================================================================
// MAIN EXECUTION - TEST PROGRAM
// ============================================================================

int main()
{
    cout << string(60, '=') << endl;
    cout << "          CPU SIMULATOR - PIPELINED EXECUTION" << endl;
    cout << "         (2-Stage: IF, EX | Hazard Detection)" << endl;
    cout << string(60, '=') << endl;

    logger1("===== CPU PIPELINED EXECUTION STARTED =====");

    // Initialize Memory and CPU
    initialize_memory();
    PC = 0x00; 

    // Initial state
    R1 = hex_to_bits("0x0");
    R2 = hex_to_bits("0x3");

    cout << "\n--- STARTING PIPELINE EXECUTION ---\n" << endl;

    // Run pipeline clock cycles
    // The loop runs until the halt_flag is set and the pipeline is empty
    while (!halt_flag || EX_valid)
    {
        pipeline_step();
        display_cpu_state();
        
        // Safety break
        if (total_cycles > 50) break;
    }

    // -------- SUMMARY --------
    double cpi = (instructions_completed > 0) ? (double)total_cycles / instructions_completed : 0;

    cout << "\n"
         << string(60, '=') << endl;
    cout << "                 EXECUTION SUMMARY" << endl;
    cout << string(60, '=') << endl;
    cout << "Total Clock Cycles:       " << total_cycles << endl;
    cout << "Instructions Completed:   " << instructions_completed << endl;
    cout << "Stall Cycles (Bubbles):   " << stall_cycles << endl;
    cout << "Flush Cycles (Jumps):     " << flush_cycles << endl;
    cout << "Cycles Per Instruction:   " << fixed << setprecision(2) << cpi << endl;
    cout << string(60, '=') << endl;
    cout << "Final Register States:" << endl;
    cout << "  R1: " << bits_to_hex(R1) << "  R2: " << bits_to_hex(R2) 
         << "  ACC: " << bits_to_hex(ACC) << endl;
    cout << string(60, '=') << dec << endl;

    logger1("===== CPU PIPELINED EXECUTION COMPLETED =====");
    return 0;
}
