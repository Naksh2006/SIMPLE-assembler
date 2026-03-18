

# SIMPLE Assembler and Emulator

## 1. Overview

This project implements a **two-pass assembler** and an **emulator** for the SIMPLE instruction set.

* The **assembler** takes a `.asm` file and converts it into machine code (`.o`)
* The **emulator** executes the generated machine code

The goal of this project is to understand how assembly code is translated and executed at a low level.

---

## 2. Language and Tools

* Language: **C++**
* Features used: `vector`, `map`, `unordered_map`, `stringstream`
* Compilation:

```bash
g++ -std=c++17 assembler.cpp -o asm
g++ -std=c++17 emulator.cpp -o emu
```

---

## 3. Project Structure

```
asm.cpp   → assembler implementation  
emu.cpp    → emulator implementation  
test1.asm        → input assembly file  
test1.o          → generated object file  
test1.lst        → listing file  
test1.log        → errors and warnings  
```

---

## 4. SIMPLE Architecture

### Registers

| Register | Description        |
| -------- | ------------------ |
| A        | accumulator        |
| B        | secondary register |
| PC       | program counter    |
| SP       | stack pointer      |

---

### Instruction Format

Each instruction is 32 bits:

```
| operand (24 bits) | opcode (8 bits) |
```

Encoding:

```cpp
instruction = ((operand & 0xFFFFFF) << 8) | opcode;
```

---

## 5. Assembler Design

### Pass 1

* Reads all lines
* Stores labels in a symbol table
* Assigns addresses to instructions
* Handles `SET` directive

Example:

```asm
val: SET 75
```

Stored as:

```
val → 75
```

---

### Pass 2

* Generates machine code
* Resolves operands (numbers or labels)
* Computes branch offsets:

```
offset = target - (current + 1)
```

---

## 6. Pseudo Instructions

### SET

Assigns a value to a label (does not generate code)

```asm
val: SET 75
```

---

### data

Stores a value in memory

```asm
x: data 10
```

---

## 7. Error Handling

The assembler detects:

* invalid mnemonic
* duplicate labels
* undefined labels
* missing operand
* unexpected operand
* invalid number format

---

## 8. Warnings Implemented

* Infinite loop:

```asm
loop: br loop
```

→ offset becomes `-1`

* Missing HALT:

```
Warning: Program has no HALT instruction
```

---

## 9. Emulator Design

The emulator simulates execution using:

```
fetch → decode → execute
```

Execution loop:

```cpp
instruction = memory[PC];
PC++;
execute(instruction);
```

---

## 10. Instruction Behavior

Example:

```asm
ldc 5
adc 3
```

Execution:

```
A = 5
A = 8
```

---

## 11. Test Programs

The project was tested with:

* basic instruction tests
* SET directive test
* triangle numbers
* fibonacci
* maximum of array
* sum of array
* bubble sort

These tests verify:

* correct label resolution
* correct offset calculation
* proper stack usage
* correct memory access

---

## 12. Important Issues Faced

### 1. SET Directive Bug

Initially, `SET` was not handled correctly due to case mismatch (`SET` vs `set`).
This caused labels to store addresses instead of values.

---

### 2. Listing File Misalignment

Machine code and source lines were mismatched due to incorrect indexing between `programLines` and `machineCode`.

---

### 3. Label Address Issue

Label-only lines had address `-1`, which caused `ffffffff` to appear in the listing file.

---

### 4. Branch Offset Confusion

Understanding correct formula:

```
offset = target - (current + 1)
```

---

### 5. Manual Number Parsing

Implemented custom parsing for:

* decimal
* octal
* hexadecimal

instead of using built-in functions.

---

### 6. Infinite Loop Detection

Detected branch instructions where offset becomes `-1`.

---

## 13. Output Files

### Object File (.o)

Binary machine code used by emulator.

---

### Listing File (.lst)

Example:

```
00000000          val:
00000000          SET 75
00000000 00004b00 ldc val
```

---

### Log File (.log)

Example:

```
Assembly successful

Warning: Line 6: branch instruction will cause infinite loop
Warning: Program has no HALT instruction
```

---

## 14. Conclusion

This project implements a working assembler and emulator for SIMPLE.

It helped in understanding:

* how assembly is translated to machine code
* how instructions are executed
* how memory and registers interact
* how branch and function calls work

---

