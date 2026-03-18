// AUTHOR: Nakshtra Goyal
// ROLL NO: 2401CS44

// Declaration of Authorship
// This project submission is my original work. All source code,
// assembly programs, test files and documentation included in
// this submission were written and tested by me.

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

// Machine configuration
const int mem_limit = 1 << 20; // memory size

// Cpu statte
struct CPU_state {

  int32_t regA; // accumulator a
  int32_t regB; // register b
  int32_t pc;   // proogram counter
  int32_t sp;   // stack pointer
};

// Instruction representation
struct Instr {
  int opcode;
  int32_t operand;
};

// Global memory
vector<int32_t> memory(mem_limit, 0);  // main memory of emulator

CPU_state cpu;   // cpu instance

// parameters to control execution
bool running = true;
long long executed = 0;

// to print the memory according to program size
int program_size = 0;

// Conver to hex
string hex32(int32_t value) {
  stringstream ss;
  ss << uppercase << hex << setw(8) << setfill('0') << (uint32_t)value;
  return ss.str();
}

// function to  extract base name filename
string get_basename(const string &filename) {
  size_t pos = filename.find_last_of('.');
  if (pos != string::npos)
    return filename.substr(0, pos);

  return filename;
}

// Reset CPU registers
void initialiseCPU(){
  cpu.regA = 0;
  cpu.regB = 0;
  cpu.pc = 0;
  cpu.sp = mem_limit - 1;
}

// load machine code from object file into memory
void loadobject(const string &file) {
  ifstream input(file, ios::binary);

  if (!input) {
    cout << "Cannot open object file" << endl;
    exit(1);
  }

  int32_t word;
  program_size = 0;

  while (input.read((char *)&word, sizeof(word))) {

    if (program_size >= mem_limit) {
      cout << "Program too large" << endl;
      break;
    }

    memory[program_size] = word;
    program_size++;
  }

  input.close();
}

// Decode 32 bit Machine instruction into opcode and operand
Instr unpack(int32_t word) {
  Instr ins;
  ins.opcode = word & 0xFF;
  ins.operand = ((int32_t)word) >> 8;
  return ins;
}

// safety check
bool validAddress(int addr) { return addr >= 0 && addr < mem_limit; }

// instruction names according to opcode
string instr_name(int opcode) {
  switch (opcode) {

  case 0:
    return "ldc";
  case 1:
    return "adc";
  case 2:
    return "ldl";
  case 3:
    return "stl";
  case 4:
    return "ldnl";
  case 5:
    return "stnl";

  case 6:
    return "add";
  case 7:
    return "sub";
  case 8:
    return "shl";
  case 9:
    return "shr";

  case 10:
    return "adj";
  case 11:
    return "a2sp";
  case 12:
    return "sp2a";

  case 13:
    return "call";
  case 14:
    return "return";

  case 15:
    return "brz";
  case 16:
    return "brlz";
  case 17:
    return "br";

  case 18:
    return "HALT";

  default:
    return "unknown";
  }
}

// print cpu register
void show_registers(const Instr &ins) {
  cout << "PC=" << hex32(cpu.pc) << " SP=" << hex32(cpu.sp)
       << " A=" << hex32(cpu.regA) << " B=" << hex32(cpu.regB) << "  "
       << instr_name(ins.opcode);
  if (ins.opcode <= 5 || ins.opcode == 10 ||
      ins.opcode >= 13 && ins.opcode <= 17) {

    cout << " " << ins.operand;
  }
  cout << endl;
}

//  Dump memory and register state file
void dumpmemory(const string &name) {
  string file = name + "_memdump.txt";
  ofstream out(file);

  if (!out) {
    cout << "Error! : Could not create dump file\n";
    return;
  }

  out << "Program Memory Dump" << endl;
  out << "\n";

  for (int i = 0; i < mem_limit; i += 4) {
    bool has_data = false;
    for (int j = 0; j < 4 && (i + j) < mem_limit; j++) {
      if (memory[i + j] != 0) {
        has_data = true;
        break;
      }
    }
    
    if (has_data) {
      out << hex32(i) << " : ";
      for (int j = 0; j < 4 && (i + j) < mem_limit; j++) {
        out << hex32(memory[i + j]) << " ";
      }
      out << "\n";
    }
  }
  // prints registers
  out << "\nRegisters\n";
  out << "A  = " << hex32(cpu.regA) << "\n";
  out << "B  = " << hex32(cpu.regB) << "\n";
  out << "PC = " << hex32(cpu.pc) << "\n";
  out << "SP = " << hex32(cpu.sp) << "\n";

  out.close();
}

// Execute one instruction

void execute_instr(const Instr &ins) {
  executed++;

  switch (ins.opcode) {
  case 0: // ldc
    cpu.regB = cpu.regA;
    cpu.regA = ins.operand;
    break;

  case 1: // adc
    cpu.regA += ins.operand;
    break;

  case 2: // ldl
  {
    int addr = cpu.sp + ins.operand;

    if (!validAddress(addr)) {
      cout << "Memory violation\n";
      running = false;
      break;
    }

    cpu.regB = cpu.regA;
    cpu.regA = memory[addr];
    break;
  }

  case 3: // stl
  {
    int addr = cpu.sp + ins.operand;

    if (!validAddress(addr)) {
      cout << "Memory violation\n";
      running = false;
      break;
    }

    memory[addr] = cpu.regA;
    cpu.regA = cpu.regB;
    break;
  }

  case 4: // ldnl
  {
    int addr = cpu.regA + ins.operand;

    if (!validAddress(addr)) {
      cout << "Memory violation\n";
      running = false;
      break;
    }

    cpu.regA = memory[addr];
    break;
  }

  case 5: // stnl
  {
    int addr = cpu.regA + ins.operand;

    if (!validAddress(addr)) {
      cout << "Memory violation\n";
      running = false;
      break;
    }

    memory[addr] = cpu.regB;
    break;
  }

  case 6: // add
    cpu.regA = cpu.regB + cpu.regA;
    break;

  case 7: // sub
    cpu.regA = cpu.regB - cpu.regA;
    break;

  case 8: // shl
    cpu.regA = cpu.regB << cpu.regA;
    break;

  case 9: // shr
    cpu.regA = cpu.regB >> cpu.regA;
    break;

  case 10: // adj
    cpu.sp += ins.operand;
    break;

  case 11: // a2sp
    cpu.sp = cpu.regA;
    cpu.regA = cpu.regB;
    break;

  case 12: // sp2a
    cpu.regB = cpu.regA;
    cpu.regA = cpu.sp;
    break;

  case 13: // call
    cpu.regB = cpu.regA;
    cpu.regA = cpu.pc;
    cpu.pc += ins.operand;
    break;

  case 14: // return
    cpu.pc = cpu.regA;
    cpu.regA = cpu.regB;
    break;

  case 15: // brz
    if (cpu.regA == 0)
      cpu.pc += ins.operand;
    break;

  case 16: // brlz
    if (cpu.regA < 0)
      cpu.pc += ins.operand;
    break;

  case 17: // br
    cpu.pc += ins.operand;
    break;

  case 18: // halt
    running = false;
    break;

  default:
    cout << "Illegal opcode encountered\n";
    running = false;
  }
}

//  Run the emulator
void start_execution(bool trace) {
  running = true;
  int limit = 1e5;

  while (running && executed < limit) {

    if (cpu.pc < 0 || cpu.pc >= mem_limit) {
      cout << "PC out of range" << endl;
      break;
    }
    int32_t word = memory[cpu.pc];
    Instr ins = unpack(word);

    // update pc to next instructions
    cpu.pc++;

    if (trace)
      show_registers(ins);

    execute_instr(ins);
  }
  if (executed >= limit) {
    cout << "Execution stopped (possible infinite loop)" << endl;
  }
}
// print supported instruction set
void print_isa() {
  cout << "Supported instructions\n\n";

  cout << "ldc adc ldl stl ldnl stnl\n";
  cout << "add sub shl shr\n";
  cout << "adj a2sp sp2a\n";
  cout << "call return\n";
  cout << "brz brlz br\n";
  cout << "HALT\n";
}

int main(int argc, char *argv[]) {
    // check if at least one argument
  if (argc < 2) {
    cout << "Usage: emulator [options] file.o\n";
    return 0;
  }

  string filename;  // object file name
  bool trace = false;   // enable instructions trace
  bool before = false;  // dump memory before execution
  bool after = false;   // dump memory after execution
  bool isa = false;      // print instruction set


  // parse command line arguments
  for (int i = 1; i < argc; i++) {
    string arg = argv[i];

    if (arg == "-trace")
      trace = true;
    else if (arg == "-before")
      before = true;
    else if (arg == "-after")
      after = true;
    else if (arg == "-isa")
      isa = true;
      // otherwise treat argument as obect file name
    else
      filename = arg;
  }
// print supported instructions if -isa option is used
  if (isa) {
    print_isa();
    return 0;
  }
  // ensure object file was provided
  if (filename.empty()) {
    cout << "No object file provided\n";
    return 0;
  }
  // load object into memory
  loadobject(filename);
  initialiseCPU();         // reset cpu registers

  string base = get_basename(filename);

  if (before)
    dumpmemory(base + "_before");
  start_execution(trace);   // start program execution
  if (after)
    dumpmemory(base + "_after");
  cout << executed << " instructions executed\n";   //print total executed instructions

  return 0;
}