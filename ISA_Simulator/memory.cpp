#include <bits/stdc++.h>
using namespace std;


/*
     Structure of memory.h
     Uint Addr , Instruction , Operand , opcode, data, valid
     certain memory locations are reserved for instructions and hence addressing 
     would start from 0X80 to 0xFF.
*/
void initialize_memory()
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

     /* 
        TEST PROGRAM: Calculate (M[0x80] + M[0x81]) and store result in M[0x82]
        Then jump to HLT.
     */

     // 1. Initial Load of R2 from memory
     main_memory[0x00] = {0x00, "LOAD R2, 0x81", {0, 0, 0, 0}, "LOAD",  0x06, "0x81", true};

     // 2. Load R1 from memory (Target for next ADD)
     main_memory[0x01] = {0x01, "LOAD R1, 0x80", {0, 0, 0, 0}, "LOAD",  0x06, "0x80", true};

     // 3. ADD instruction (Uses R1 and R2)
     // This will trigger a STALL because R1 was just loaded in the previous instruction.
     main_memory[0x02] = {0x02, "ADD R1, R2",   {0, 0, 0, 0}, "ADD",   0x01, "0x00", true};

     // 4. STORE the result (ACC holds 5+3=8) back to memory at 0x82
     main_memory[0x03] = {0x03, "STORE ACC, 0x82", {0, 0, 0, 0}, "STORE", 0x07, "0x82", true};

     // 5. Jump to the finish
     main_memory[0x04] = {0x04, "JMP 0x06",      {0, 0, 0, 0}, "JMP",   0x08, "0x06", true};

     // 6. This instruction should be FLUSHED
     main_memory[0x05] = {0x05, "SUB R3, R4",   {0, 0, 0, 0}, "SUB",   0x02, "0x00", true};

     // 7. Finish
     main_memory[0x06] = {0x06, "HLT",           {0, 0, 0, 0}, "HLT",   0x10, "0x00", true};

     // --- Data Section ---
     main_memory[0x80] = {0x80, "DATA: 0x05", {0, 0, 0, 0}, "DATA", 0x00, "0x05", true}; // Input A: 5
     main_memory[0x81] = {0x81, "DATA: 0x03", {0, 0, 0, 0}, "DATA", 0x00, "0x03", true}; // Input B: 3
     main_memory[0x82] = {0x82, "DATA: 0x00", {0, 0, 0, 0}, "DATA", 0x00, "0x00", true}; // Result placeholder
}

// Read memory at address
memoryElement read_memory(unsigned int address)
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
void write_memory(unsigned int address, memoryElement element)
{
     if (address < 256)
          main_memory[address] = element;
     else
          cerr << "Memory write out of bounds: " << address << endl;
}

// Display memory contents
void display_memory(unsigned int start_address = 0, unsigned int end_address = 255)
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

// Display memory segment (program or data)
void display_program_section()
{
     cout << "\n=== PROGRAM SECTION (0x00 - 0x0F) ===" << endl;
     display_memory(0x00, 0x0F);
}

void display_data_section()
{
     cout << "\n=== DATA SECTION (0x80 - 0x8F) ===" << endl;
     display_memory(0x80, 0x8F);
}
