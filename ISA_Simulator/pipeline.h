#ifndef PIPELINE_H
#define PIPELINE_H

#include <cstdint>
#include <string>

using namespace std;

// IF/EX Pipeline Register
struct IFEX_Register
{
    bool valid;           // Is this instruction valid?
    uint8_t opcode;       // Opcode (4 bits)
    uint8_t operand;      // Operand/Register address (4 bits)
    uint8_t address_data; // Immediate value or memory address
    uint8_t pc;           // PC of this instruction
    string mnemonic;      // Mnemonic for debugging
    
    // For hazard detection
    uint8_t dest_reg;     // Destination register (for LD instructions)
    bool is_load;         // Is this a load instruction?
    
    // For forwarding (Assignment IV Part A)
    bool produces_result; // Does this instruction produce a result?
    uint8_t result_value; // The result value to forward
    bool result_ready;    // Is the result ready to forward?
};

// Forwarding Unit State (Assignment IV Part A)
struct ForwardingUnit
{
    bool forward_enabled;    // Is forwarding enabled?
    bool forward_active;     // Is forwarding happening this cycle?
    uint8_t forward_reg;     // Register being forwarded
    uint8_t forward_value;   // Value being forwarded
};

extern ForwardingUnit forwarding_unit;

// Global pipeline register
extern IFEX_Register ifex_reg;

// Pipeline control flags
extern bool stall_flag;       // Insert stall this cycle
extern bool flush_flag;       // Flush pipeline this cycle

// Cache stall management
extern int cache_stall_remaining;  // Remaining cache stall cycles

// Initialize pipeline
void initialize_pipeline();

// IF Stage: Instruction Fetch
void instruction_fetch();

// EX Stage: Execute / Memory / Writeback
void execute_writeback();

// Hazard Detection
bool detect_load_use_hazard();

// Forwarding Unit (Assignment IV Part A)
// Check if forwarding is possible for the current IF instruction
bool check_forwarding(uint8_t required_reg, uint8_t &forwarded_value);

// Check if the EX instruction can forward its result
bool can_forward();

// Insert stall (bubble)
void insert_stall();

// Flush pipeline
void flush_pipeline();

// Display pipeline state
void display_pipeline_state(int cycle);

#endif // PIPELINE_H

