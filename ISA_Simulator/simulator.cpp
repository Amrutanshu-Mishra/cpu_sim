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
    cout << "          CPU SIMULATOR - EXECUTION PROGRAM" << endl;
    cout << "         (LOAD, ADD, SUB Instructions)" << endl;
    cout << string(60, '=') << endl;

    logger1("===== CPU SIMULATOR EXECUTION STARTED =====");

    // -------- PROGRAM START --------
    cout << "\n--- PHASE 1: LOADING DATA INTO REGISTERS ---\n"
         << endl;

    LOAD_instruction(0x00, "R1", "0x05"); // Load 5 into R1
    LOAD_instruction(0x01, "R2", "0x03"); // Load 3 into R2
    LOAD_instruction(0x02, "R3", "0x0A"); // Load 10 into R3
    LOAD_instruction(0x03, "R4", "0x02"); // Load 2 into R4

    display_cpu_state();

    // -------- ADDITION OPERATIONS --------
    cout << "\n--- PHASE 2: PERFORMING ADDITION OPERATIONS ---\n"
         << endl;

    ADD_instruction(0x04, "ACC", "R1", "R2"); // ACC = R1 + R2 = 5 + 3 = 8
    display_cpu_state();

    ADD_instruction(0x05, "R1", "R3", "R4"); // R1 = R3 + R4 = 10 + 2 = 12
    display_cpu_state();

    // -------- SUBTRACTION OPERATIONS --------
    cout << "\n--- PHASE 3: PERFORMING SUBTRACTION OPERATIONS ---\n"
         << endl;

    SUB_instruction(0x06, "R2", "R3", "R1"); // R2 = R3 - R1
    display_cpu_state();

    SUB_instruction(0x07, "ACC", "ACC", "R4"); // ACC = ACC - R4
    display_cpu_state();

    // -------- FINAL OPERATIONS --------
    cout << "\n--- PHASE 4: FINAL COMPUTATIONS ---\n"
         << endl;

    ADD_instruction(0x08, "ACC", "ACC", "R2"); // ACC = ACC + R2
    display_cpu_state();

    // -------- SUMMARY --------
    cout << "\n"
         << string(60, '=') << endl;
    cout << "                 EXECUTION SUMMARY" << endl;
    cout << string(60, '=') << endl;
    cout << "Instructions Executed:    " << PC << " instructions" << endl;
    cout << "Final Register States:" << endl;
    cout << "  R1: " << bits_to_hex(R1) << " (decimal: " << bits_to_decimal(R1) << ")" << endl;
    cout << "  R2: " << bits_to_hex(R2) << " (decimal: " << bits_to_decimal(R2) << ")" << endl;
    cout << "  R3: " << bits_to_hex(R3) << " (decimal: " << bits_to_decimal(R3) << ")" << endl;
    cout << "  R4: " << bits_to_hex(R4) << " (decimal: " << bits_to_decimal(R4) << ")" << endl;
    cout << "  ACC: " << bits_to_hex(ACC) << " (decimal: " << bits_to_decimal(ACC) << ")" << endl;
    cout << string(60, '=') << endl;

    logger1("===== CPU SIMULATOR EXECUTION COMPLETED =====");
    cout << "\nExecution log saved to ISA_Simulator/log.txt" << endl;

    return 0;
}
