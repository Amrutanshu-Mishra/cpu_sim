#include "pipeline.h"
#include "registers.h"
#include "data_memory.h"
#include "performance.h"
#include "log_handler.h"
#include "cache.h"
#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>

using namespace std;

// Forward declarations for memory functions from memory.cpp
struct memoryElement
{
    unsigned int address;
    string instruction;
    bool operand[4];
    string mnemonic;
    unsigned char opcode;
    string data;
    bool valid;
};

extern memoryElement main_memory[256];
void initialize_memory();
memoryElement read_memory(unsigned int address);
void write_memory(unsigned int address, memoryElement element);
void display_program_section();


// IF/EX Pipeline Register
IFEX_Register ifex_reg;

// Forwarding Unit (Assignment IV Part A)
ForwardingUnit forwarding_unit;

// Pipeline control flags
bool stall_flag = false;
bool flush_flag = false;

// Cache stall management
int cache_stall_remaining = 0;

// Initialize pipeline
void initialize_pipeline()
{
    ifex_reg.valid = false;
    ifex_reg.opcode = 0;
    ifex_reg.operand = 0;
    ifex_reg.address_data = 0;
    ifex_reg.pc = 0;
    ifex_reg.mnemonic = "NOP";
    ifex_reg.dest_reg = 0;
    ifex_reg.is_load = false;
    
    // Initialize forwarding fields
    ifex_reg.produces_result = false;
    ifex_reg.result_value = 0;
    ifex_reg.result_ready = false;
    
    // Initialize forwarding unit
    forwarding_unit.forward_enabled = true;  // Enable forwarding by default
    forwarding_unit.forward_active = false;
    forwarding_unit.forward_reg = 0;
    forwarding_unit.forward_value = 0;
    
    stall_flag = false;
    flush_flag = false;
    cache_stall_remaining = 0;
}

// Helper function to decode instruction from memory
struct DecodedInstruction
{
    uint8_t opcode;
    uint8_t operand;
    uint8_t address_data;
    string mnemonic;
};

DecodedInstruction decode_instruction(uint8_t pc_value)
{
    DecodedInstruction decoded;
    memoryElement mem = read_memory(pc_value);
    
    decoded.opcode = mem.opcode;
    decoded.mnemonic = mem.mnemonic;
    
    // Extract operand (register) from instruction
    // Assuming operand array represents register number
    decoded.operand = 0;
    for (int i = 0; i < 4; i++)
        decoded.operand |= (mem.operand[i] << (3 - i));
    
    // Extract address/immediate data
    // Convert hex string to value
    try {
        decoded.address_data = (uint8_t)stoi(mem.data, nullptr, 16);
    } catch (...) {
        decoded.address_data = 0;
    }
    
    return decoded;
}

// Check if the EX instruction can forward its result (Assignment IV Part A)
// ALU instructions produce results immediately, LOAD instructions produce results after MEM stage
bool can_forward()
{
    if (!ifex_reg.valid)
        return false;
    
    // ALU instructions (ADD, SUB, MUL, DIV) can forward their results
    // They produce results at the end of EX stage
    uint8_t opcode = ifex_reg.opcode;
    
    switch (opcode)
    {
        case 0x01: // ADD
        case 0x02: // SUB
        case 0x03: // MUL
        case 0x04: // DIV
            return ifex_reg.result_ready;
        
        case 0x0D: // LD - can forward after load completes
            return ifex_reg.result_ready;
        
        default:
            return false;
    }
}

// Check if forwarding is possible for the current IF instruction (Assignment IV Part A)
bool check_forwarding(uint8_t required_reg, uint8_t &forwarded_value)
{
    if (!forwarding_unit.forward_enabled)
        return false;
    
    if (!ifex_reg.valid || !ifex_reg.produces_result)
        return false;
    
    // Check if EX stage instruction writes to the required register
    if (ifex_reg.dest_reg == required_reg && ifex_reg.result_ready)
    {
        forwarded_value = ifex_reg.result_value;
        
        cout << "  [FORWARDING] Forwarding R" << (int)required_reg 
             << " = 0x" << hex << (int)forwarded_value << dec 
             << " from EX stage" << endl;
        
        logger1("FORWARDING: R" + to_string(required_reg) + 
               " = 0x" + to_string(forwarded_value) + " forwarded from EX stage");
        
        increment_forwarding();
        forwarding_unit.forward_active = true;
        forwarding_unit.forward_reg = required_reg;
        forwarding_unit.forward_value = forwarded_value;
        
        return true;
    }
    
    return false;
}

// IF Stage: Instruction Fetch
void instruction_fetch()
{
    if (halt_flag)
    {
        return;  // Don't fetch if halted
    }
    
    if (stall_flag)
    {
        // Stall: don't fetch new instruction, don't increment PC
        cout << "  [IF] STALL - No new fetch" << endl;
        return;
    }
    
    if (flush_flag)
    {
        // Flush: invalidate the fetched instruction
        cout << "  [IF] FLUSH - Discarding fetched instruction" << endl;
        return;
    }
    
    // Fetch instruction from instruction memory
    DecodedInstruction decoded = decode_instruction(PC);
    
    cout << "  [IF] Fetching from PC=" << (int)PC 
         << " | Instruction: " << decoded.mnemonic << endl;
    
    // Log instruction fetch
    logger1("IF Stage: Fetching instruction from PC=" + to_string(PC) + " | Mnemonic: " + decoded.mnemonic);
    
    // Move current IF instruction to EX stage (update pipeline register)
    // But first save the old IFEX for execution
    // Actually, we need to execute first, then fetch - reordering in main loop
    
    // Increment PC (unless stalling)
    PC++;
}

// EX Stage: Execute / Memory / Writeback with Forwarding and Cache
void execute_writeback()
{
    // Reset forwarding state for this cycle
    forwarding_unit.forward_active = false;
    
    if (!ifex_reg.valid)
    {
        cout << "  [EX] Bubble (no valid instruction)" << endl;
        return;
    }
    
    cout << "  [EX] Executing: " << ifex_reg.mnemonic 
         << " (opcode=0x" << hex << (int)ifex_reg.opcode << dec << ")" << endl;
    
    uint8_t opcode = ifex_reg.opcode;
    uint8_t reg = ifex_reg.operand;
    uint8_t data = ifex_reg.address_data;
    
    // Reset result fields
    ifex_reg.produces_result = false;
    ifex_reg.result_ready = false;
    ifex_reg.result_value = 0;
    
    // Execute based on opcode
    switch (opcode)
    {
        case 0x01: // ADD
        {
            uint8_t val1 = read_register(reg);
            uint8_t val2 = read_register((reg + 1) % 16);
            uint8_t result = val1 + val2;
            write_register(reg, result);
            
            // Set up forwarding info
            ifex_reg.produces_result = true;
            ifex_reg.result_value = result;
            ifex_reg.result_ready = true;
            ifex_reg.dest_reg = reg;
            
            cout << "    ADD R" << (int)reg << ", R" << (int)((reg+1)%16) 
                 << " -> R" << (int)reg << " = " << (int)result << endl;
            increment_instruction();
            break;
        }
        
        case 0x02: // SUB
        {
            uint8_t val1 = read_register(reg);
            uint8_t val2 = read_register((reg + 1) % 16);
            uint8_t result = val1 - val2;
            write_register(reg, result);
            
            // Set up forwarding info
            ifex_reg.produces_result = true;
            ifex_reg.result_value = result;
            ifex_reg.result_ready = true;
            ifex_reg.dest_reg = reg;
            
            cout << "    SUB R" << (int)reg << ", R" << (int)((reg+1)%16) 
                 << " -> R" << (int)reg << " = " << (int)result << endl;
            increment_instruction();
            break;
        }
        
        case 0x03: // MUL
        {
            uint8_t val1 = read_register(reg);
            uint8_t val2 = read_register((reg + 1) % 16);
            uint8_t result = val1 * val2;
            write_register(reg, result);
            
            // Set up forwarding info
            ifex_reg.produces_result = true;
            ifex_reg.result_value = result;
            ifex_reg.result_ready = true;
            ifex_reg.dest_reg = reg;
            
            cout << "    MUL R" << (int)reg << ", R" << (int)((reg+1)%16) 
                 << " -> R" << (int)reg << " = " << (int)result << endl;
            increment_instruction();
            break;
        }
        
        case 0x04: // DIV
        {
            uint8_t val1 = read_register(reg);
            uint8_t val2 = read_register((reg + 1) % 16);
            if (val2 != 0)
            {
                uint8_t result = val1 / val2;
                write_register(reg, result);
                
                // Set up forwarding info
                ifex_reg.produces_result = true;
                ifex_reg.result_value = result;
                ifex_reg.result_ready = true;
                ifex_reg.dest_reg = reg;
                
                cout << "    DIV R" << (int)reg << ", R" << (int)((reg+1)%16) 
                     << " -> R" << (int)reg << " = " << (int)result << endl;
            }
            else
            {
                cout << "    DIV by zero error!" << endl;
            }
            increment_instruction();
            break;
        }
        
        case 0x0D: // LD (Load from data memory via CACHE)
        {
            MAR = data;
            
            // Use cache for memory access (Assignment IV Part B)
            bool cache_hit;
            int stall_cycles;
            MDR = cache_read(MAR, cache_hit, stall_cycles);
            
            // If cache miss, we need to stall
            if (!cache_hit && stall_cycles > 0)
            {
                cache_stall_remaining = stall_cycles;
                cout << "    [CACHE] Miss penalty: stalling for " << stall_cycles << " cycles" << endl;
                logger1("CACHE MISS: Stalling pipeline for " + to_string(stall_cycles) + " cycles");
            }
            
            write_register(reg, MDR);
            
            // Set up forwarding info for LOAD result
            ifex_reg.produces_result = true;
            ifex_reg.result_value = MDR;
            ifex_reg.result_ready = true;  // Result is ready after this cycle
            ifex_reg.dest_reg = reg;
            
            cout << "    LD R" << (int)reg << ", [" << (int)data << "]"
                 << " -> R" << (int)reg << " = 0x" << hex << (int)MDR << dec << endl;
            
            // Log load operation
            logger1("EX Stage: LD R" + to_string(reg) + ", [" + to_string(data) + "] -> R" + 
                   to_string(reg) + " = 0x" + to_string(MDR));
            
            increment_instruction();
            break;
        }
        
        case 0x0E: // ST (Store to data memory via CACHE)
        {
            MAR = data;
            MDR = read_register(reg);
            
            // Use cache for memory write (Assignment IV Part B)
            int stall_cycles = cache_write(MAR, MDR);
            
            if (stall_cycles > 0)
            {
                cache_stall_remaining = stall_cycles;
                cout << "    [CACHE] Write miss: stalling for " << stall_cycles << " cycles" << endl;
                logger1("CACHE WRITE MISS: Stalling pipeline for " + to_string(stall_cycles) + " cycles");
            }
            
            cout << "    ST R" << (int)reg << ", [" << (int)data << "]"
                 << " -> MEM[" << (int)data << "] = 0x" << hex << (int)MDR << dec << endl;
            
            // Log store operation
            logger1("EX Stage: ST R" + to_string(reg) + ", [" + to_string(data) + "] -> MEM[" + 
                   to_string(data) + "] = 0x" + to_string(MDR));
            
            increment_instruction();
            break;
        }
        
        case 0x08: // JMP (Jump)
        {
            PC = data;
            flush_flag = true;
            cout << "    JMP to 0x" << hex << (int)data << dec << endl;
            increment_instruction();
            break;
        }
        
        case 0x10: // HALT
        case 0x0F: // HALT (alternative opcode)
        {
            halt_flag = true;
            cout << "    HALT - Stopping execution" << endl;
            increment_instruction();
            break;
        }
        
        default:
        {
            cout << "    Unknown opcode: 0x" << hex << (int)opcode << dec << endl;
            increment_instruction();
            break;
        }
    }
}

// Detect Load-Use Hazard with Forwarding Check (Assignment IV Part A)
bool detect_load_use_hazard()
{
    // Check if EX stage has a LOAD instruction
    if (!ifex_reg.valid || !ifex_reg.is_load)
        return false;
    
    // Get the instruction in IF stage
    if (halt_flag)
        return false;
    
    DecodedInstruction if_inst = decode_instruction(PC);
    
    // Check if IF instruction uses the register that EX LOAD is writing to
    if (if_inst.operand == ifex_reg.dest_reg)
    {
        // With forwarding enabled, check if we can forward
        if (forwarding_unit.forward_enabled && ifex_reg.result_ready)
        {
            // Load has completed, can forward - NO STALL needed
            cout << "  [FORWARDING] Load-Use hazard resolved by forwarding R" 
                 << (int)ifex_reg.dest_reg << endl;
            logger1("FORWARDING: Load-Use hazard avoided - forwarding R" + 
                   to_string(ifex_reg.dest_reg));
            return false;  // No stall needed!
        }
        
        // Cannot forward (load not complete), must stall
        cout << "  [HAZARD] Load-Use detected: LD writes R" << (int)ifex_reg.dest_reg
             << ", next instruction uses R" << (int)if_inst.operand << endl;
        return true;
    }
    
    return false;
}

// Insert stall
void insert_stall()
{
    cout << "  [PIPELINE] Inserting STALL cycle" << endl;
    stall_flag = true;
    increment_stall();
    
    // Insert bubble in EX stage (invalidate IFEX)
    ifex_reg.valid = false;
    ifex_reg.mnemonic = "BUBBLE";
}

// Flush pipeline
void flush_pipeline()
{
    cout << "  [PIPELINE] Flushing IF stage" << endl;
    flush_flag = true;
    increment_flush();
    
    // Invalidate IFEX register
    ifex_reg.valid = false;
    ifex_reg.mnemonic = "FLUSHED";
}

// Display pipeline state
void display_pipeline_state(int cycle)
{
    cout << "\n--- Cycle " << cycle << " ---" << endl;
    cout << "IF Stage: PC=" << (int)PC << endl;
    cout << "EX Stage: " << (ifex_reg.valid ? ifex_reg.mnemonic : "EMPTY") << endl;
    
    if (forwarding_unit.forward_active)
    {
        cout << "Forwarding: R" << (int)forwarding_unit.forward_reg 
             << " = 0x" << hex << (int)forwarding_unit.forward_value << dec << endl;
    }
}

// Move instruction from IF to EX (update pipeline register)
void update_pipeline_register()
{
    if (halt_flag)
        return;
    
    if (stall_flag)
    {
        // On stall, insert bubble
        ifex_reg.valid = false;
        stall_flag = false;  // Reset for next cycle
        return;
    }
    
    if (flush_flag)
    {
        // On flush, invalidate
        ifex_reg.valid = false;
        flush_flag = false;  // Reset for next cycle
        return;
    }
    
    // Fetch and decode instruction at PC
    DecodedInstruction decoded = decode_instruction(PC);
    
    // Update pipeline register
    ifex_reg.valid = true;
    ifex_reg.opcode = decoded.opcode;
    ifex_reg.operand = decoded.operand;
    ifex_reg.address_data = decoded.address_data;
    ifex_reg.pc = PC;
    ifex_reg.mnemonic = decoded.mnemonic;
    
    // Check if this is a load instruction
    ifex_reg.is_load = (decoded.opcode == 0x0D);
    ifex_reg.dest_reg = decoded.operand;
    
    // Reset forwarding fields for new instruction
    ifex_reg.produces_result = false;
    ifex_reg.result_value = 0;
    ifex_reg.result_ready = false;
}
