#include <bits/stdc++.h>
using namespace std;

#define MEM_SIZE 256

struct CPU
{
     uint8_t R[4];
     uint8_t ACC;
     uint8_t PC;
     uint8_t IR;
     uint8_t Z;
     uint8_t C;
};

struct ControlSignals
{
     bool PC_write;
     bool IR_write;
     bool ACC_write;
     bool REG_write;
     bool MEM_read;
     bool MEM_write;
     int ALU_op;
     int ALU_src;
     int PC_src;
};

enum CPUState
{
     S_FETCH,
     S_DECODE,
     S_EXEC,
     S_WB,
     S_HALT
};

uint8_t MEMORY[MEM_SIZE];

enum
{
     ALU_ADD,
     ALU_SUB,
     ALU_MUL,
     ALU_DIV,
     ALU_POW,
     ALU_CMP
};

// ================= UTILITIES =================
void reset_cpu(CPU &cpu)
{
     memset(&cpu, 0, sizeof(cpu));
}

uint8_t alu_execute(uint8_t a, uint8_t b, int alu_op, CPU &cpu)
{
     uint16_t res = 0;

     switch (alu_op)
     {
     case ALU_ADD:
          res = a + b;
          break;
     case ALU_SUB:
          res = a - b;
          break;
     case ALU_MUL:
          res = a * b;
          break;
     case ALU_DIV:
          res = (b == 0 ? 0 : a / b);
          break;
     case ALU_POW:
          res = pow(a, b);
          break;
     case ALU_CMP:
          res = a - b;
          break;
     }

     cpu.C = (res > 255);
     uint8_t out = res & 0xFF;
     cpu.Z = (out == 0);
     return out;
}

void generate_control_signals(CPU &cpu, CPUState state, ControlSignals &ctrl)
{
     memset(&ctrl, 0, sizeof(ctrl));
     uint8_t opcode = cpu.IR >> 4;

     if (state == S_FETCH)
     {
          ctrl.MEM_read = 1;
          ctrl.IR_write = 1;
          ctrl.PC_write = 1;
     }
     else if (state == S_EXEC)
     {
          if (opcode >= 0x4 && opcode <= 0x9)
          {
               ctrl.ALU_op = opcode - 0x4;
               ctrl.ACC_write = 1;
          }
          else if (opcode == 0x1)
               ctrl.ACC_write = 1;
          else if (opcode == 0x2)
               ctrl.REG_write = 1;
          else if (opcode == 0x3)
               ctrl.ACC_write = 1;
          else if (opcode == 0xA || opcode == 0xB || opcode == 0xC)
               ctrl.PC_write = 1;
     }
}

void apply_control_signals(CPU &cpu, ControlSignals &ctrl)
{
     uint8_t opcode = cpu.IR >> 4;
     uint8_t operand = cpu.IR & 0x0F;
     uint8_t &Rn = cpu.R[operand & 3];

     if (ctrl.IR_write)
          cpu.IR = MEMORY[cpu.PC];

     if (ctrl.PC_write && !ctrl.PC_src)
          cpu.PC++;

     if (ctrl.ALU_op || opcode == 0x9)
     {
          cpu.ACC = alu_execute(cpu.ACC, Rn, ctrl.ALU_op, cpu);
     }

     if (ctrl.ACC_write && opcode == 0x1)
          cpu.ACC = Rn;

     if (ctrl.ACC_write && opcode == 0x3)
          cpu.ACC = operand;

     if (ctrl.REG_write)
          Rn = cpu.ACC;

     if ((opcode == 0xA) ||
         (opcode == 0xB && cpu.Z) ||
         (opcode == 0xC && cpu.C))
          cpu.PC = operand;
}

void load_program(const string &file)
{
     ifstream fin(file);
     int x, addr = 0;
     while (fin >> hex >> x)
          MEMORY[addr++] = x;
}

void log_control_signals(ofstream &log, ControlSignals &c)
{
     log << " | CTRL["
         << "PCw=" << c.PC_write << " "
         << "IRw=" << c.IR_write << " "
         << "ACCw=" << c.ACC_write << " "
         << "REGw=" << c.REG_write << " "
         << "MEMr=" << c.MEM_read << " "
         << "MEMw=" << c.MEM_write << " "
         << "ALUop=" << c.ALU_op << " "
         << "ALUsrc=" << c.ALU_src << " "
         << "PCsrc=" << c.PC_src
         << "]";
}
int main()
{
     CPU cpu;
     ControlSignals ctrl;
     CPUState state = S_FETCH;

     reset_cpu(cpu);
     load_program("program.hex");

     ofstream log("cpu_log.txt");

     bool running = true;
     int cycle = 0;

     while (running)
     {
          generate_control_signals(cpu, state, ctrl);

          log << "Cycle " << setw(3) << cycle++
              << " | State:" << state
              << " | PC:" << int(cpu.PC)
              << " IR:" << hex << setw(2) << setfill('0') << int(cpu.IR) << dec
              << " ACC:" << setw(3) << int(cpu.ACC)
              << " R0:" << setw(3) << int(cpu.R[0])
              << " R1:" << setw(3) << int(cpu.R[1])
              << " R2:" << setw(3) << int(cpu.R[2])
              << " R3:" << setw(3) << int(cpu.R[3])
              << " Z:" << int(cpu.Z)
              << " C:" << int(cpu.C);

          log_control_signals(log, ctrl);
          log << "\n";

          apply_control_signals(cpu, ctrl);

          switch (state)
          {
          case S_FETCH:
               state = S_DECODE;
               break;
          case S_DECODE:
               state = S_EXEC;
               break;
          case S_EXEC:
               state = S_WB;
               break;
          case S_WB:
               if ((cpu.IR >> 4) == 0xF)
                    state = S_HALT;
               else
                    state = S_FETCH;
               break;
          case S_HALT:
               running = false;
               break;
          }
     }

     log << "\nProgram finished.\n";
     log.close();

     cout << "Execution complete. Full trace in cpu_log.txt\n";
}
