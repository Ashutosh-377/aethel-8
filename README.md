# Aethel-8
> A cycle-accurate, bare-metal 8-bit virtual computer architecture simulation, assembler toolchain, and custom terminal Operating System environment built from scratch in under 300 lines of unbloated C.

![Aethel-8 graphical logo](logo.png)

## 🔬 System Engineering Overview

Aethel-8 is an educational software-defined processor designed to demystify the hardware-software abstraction layer. Operating under a strict **Von Neumann Architecture** paradigm—where operational code and dynamic runtime data occupy a shared, unified memory highway—the entire machine executes natively in a controlled, constraints-driven sandbox.

### ⚡ System Technical Specifications

* **Word Size:** 8-bit fixed-width execution environment (`uint8_t`).
* **Memory Map:** Isolated 256-byte volatile RAM matrix (`0x00` to `0xFF`).
* **Registers:**
* `A` (8-bit Accumulator: primary ALU operand destination).
* `B` (8-bit Secondary storage register).
* `PC` (8-bit Program Counter: tracks the sequential memory fetch pointer).
* `ZF` (1-bit Hardware status Zero Flag: toggles based on ALU mathematical outcomes).



---

## 🛠️ Infrastructure Layers

The system collapses the traditional, highly abstracted computer science stack into three cleanly linked modules:

1. **The Virtual Hardware (ALU & Control Unit):** Loops through an explicit, cycle-accurate **Fetch-Decode-Execute** pipeline. It features an Arithmetic Logic Unit capable of processing binary addition, subtraction, multiplication, underflow/overflow handling, and reactive hardware flag toggles.
2. **The Structural Compiler (Two-Pass Mini-Assembler):** Evaluates clean text string mnemonics, tokenizes arguments via pointer evaluation, maps memory spacing dynamically, and compiles human-readable syntax into absolute binary machine code.
3. **The Shell (Toy OS Operating System):** A persistent REPL terminal environment providing bare-metal diagnostic functions including historic `PEEK` and `POKE` memory overrides, along with a complete 16x16 hexadecimal layout `DUMP` utility.

---

## 💻 Instruction Set Architecture (ISA)

Aethel-8 supports up to 256 structural opcodes. The current hardware matrix implements:

| Opcode | Mnemonic | Size (Bytes) | Operational Description |
| --- | --- | --- | --- |
| `0x00` | HALT | 1 | Terminates automated processor clock execution cycles. |
| `0x01` | LDA  | 2 | Loads an immediate 8-bit scalar value into Register A. |
| `0x02` | LDB  | 2 | Loads an immediate 8-bit scalar value into Register B. |
| `0x03` | ADD | 1 | Adds Reg B to Reg A via ALU. Updates `ZF`. |
| `0x04` | SUB | 1 | Subtracts Reg B from Reg A via ALU. Updates `ZF`. |
| `0x05` | MUL | 1 | Multiplies Reg A by Reg B via ALU. Updates `ZF`. |
| `0x06` | JMP  | 2 | Unconditional branch. Forces `PC` directly to target address. |
| `0x07` | JNZ  | 2 | Conditional branch. Jumps to target address if `ZF == 0`. |
| `0x08` | STA  | 2 | Flashes the current contents of Register A directly out to RAM. |
| `0x09` | STB  | 2 | Flashes the current contents of Register B directly out to RAM. |

---

## 🚀 Installation & Execution

The repository includes an automated building utility script (`Makefile`). To compile and launch the machine on any POSIX terminal environment:

```bash
# Clone the architecture sandbox
git clone https://github.com/Ashutosh-377/aethel-8.git
cd aethel-8

# Compile the entire stack via the Makefile
make

# Boot into the Aethel-8 Hardware Environment
./aethel8

```

### Writing Your First Computational Loop

Boot the machine and type these instructions line-by-line to program a live algorithmic countdown circuit:

```text
aethel8> LDA 3
aethel8> LDB 1
aethel8> SUB
aethel8> JNZ 4
aethel8> HALT
aethel8> RUNALL
aethel8> DUMP
```

---

## 🔮 Future Architecture Roadmap

* Would like to advance into making `16-bit` and `32-bit` computer systems complete virtually.
