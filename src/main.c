#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aethel8.h"

// Wipe the system clean and reset registers to zero to avoid garbage data bugs
void reset_system(Aethel8_CPU *cpu) {
    cpu->A = 0;
    cpu->B = 0;
    cpu->PC = 0;

    for (int i = 0; i < MEMORY_SIZE; i++){
        memory[i] = 0;
    }
    
    printf("[SYSTEM] Hardware reset complete. Core initialized.\n");
}

// The heart of the CPU: One full Fetch -> Decode -> Execute sequence
void execute_cycle(Aethel8_CPU *cpu) {
    // FETCH: Grab whatever byte the Program Counter is pointing at right now
    uint8_t opcode = memory[cpu->PC];
    printf("[FETCH] PC: 0x%02X | Opcode: 0x%02X\n", cpu->PC, opcode);
    cpu->PC++; // Instantly push PC forward so it points to the next byte/argument

    // DECODE & EXECUTE: Figure out what the byte means and run the circuitry
    switch (opcode) {
        case OP_LD_A_IMMD: {
            // Read the raw value hiding in the very next byte of memory
            uint8_t value = memory[cpu->PC];
            cpu->A = value;
            printf("[EXECUTE] Loaded immediate value 0x%02X into Register A\n", value);
            cpu->PC++; // Step past the value byte so the CPU doesn't try to run it as code next loop
            break;
        }

        case OP_LD_B_IMMD: {
            uint8_t value = memory[cpu->PC];
            cpu->B = value;
            printf("[EXECUTE] Loaded immideate value 0x%02X into RegisterB.\n", value);
            cpu->PC++;
            break;
        }

        case OP_ADD_B_A: {
            cpu->A = cpu->A + cpu->B;
            // Hardware Flag calculation: if the result is a flat 0, light up the ZF bit
            cpu->ZF = (cpu->A == 0) ? 1 : 0;
            printf("[EXECUTE] ALU ADD: Added Reg B to Reg A. New Reg A = 0x%02X.\n", cpu->A);
            break;
        }

        case OP_SUB_B_A: {
            cpu->A = cpu->A - cpu->B;
            cpu->ZF = (cpu->A == 0) ? 1 : 0; // If Reg A drops to 0, this flag tells JNZ loops to break
            printf("[EXECUTE] ALU SUB: Subtracted Reg B from Reg A. New Register A value = 0x%02X.\n", cpu->A);
            break;
        }

        case OP_MUL_B_A: {
            cpu->A = cpu->A * cpu->B;
            cpu->ZF = (cpu->A == 0) ? 1 : 0;
            printf("[EXECUTE] ALU MUL: Multiplied Reg A with Reg B. New Reg A = 0x%02X.\n", cpu->A);
            break;
        }

        case OP_JMP: {
            // Hard teleport: force the PC to overwrite itself with the target address
            uint8_t target_address = memory[cpu->PC];
            cpu->PC = target_address;
            printf("[EXECUTE] JUMP: PC forced to 0x%02X\n", target_address);
            break;
        }

        case OP_JNZ: {
            uint8_t target_address = memory[cpu->PC];
            // If ZF is 0, it means our loop counter isn't zero yet, so jump backwards!
            if (cpu->ZF == 0){
                cpu->PC = target_address;
                printf("[EXECUTE] JNZ: Branch Taken. Jumping to 0x%02X\n", target_address);
            } else {
                // Counter hit 0! Skip over the target address byte and keep moving forward
                cpu->PC++;
                printf("[EXECUTE] JNZ: Branch NOT Taken (ZF is 1). Stepping past jump.\n");
            }
            break;
        }

        case OP_STA_ADDR: {
            // Pull the data out of Register A and throw it straight into a target RAM slot
            uint8_t target_address = memory[cpu->PC];
            memory[target_address] = cpu->A;
            printf("[EXECUTE] STA: Stored Reg A (0x%02X) inot Memory Address 0x%02X\n", cpu->A, target_address);
            cpu->PC++;
            break;
        }

        case OP_STB_ADDR: {
            uint8_t target_address = memory[cpu->PC];
            memory[target_address] = cpu->B;
            printf("[EXECUTE] STB: Stored Reg B (0x%02X) inot Memory Address 0x%02X\n", cpu->B, target_address);
            cpu->PC++;
            break;
        }

        case OP_HALT: {
            printf("[EXECUTE] HALT: CPU execution halted.\n");
            break;
        }
        default:
            // Safety fallback: if we hit a weird byte we didn't define, kill execution before it breaks
            printf("[ERROR] Unknown opcode 0x%02X at PC 0x%02X. Halting.\n", opcode, cpu->PC - 1);
            break;
    }
}

// Our built-in compiler: chops up text input like "LDA 5" and turns it into raw numbers inside memory
int assemble_line(const char *line, uint8_t address) {
    char line_copy[100];
    strncpy(line_copy, line, sizeof(line_copy));
    
    // Grab the first word (the command mnemonic like LDA, ADD, etc.)
    char *mnemonic = strtok(line_copy, " ");

    if (mnemonic == NULL) {
        return 0; // Empty line typed, skip it
    }

    if (strcmp(mnemonic, "LDA") == 0) {
        char *arg = strtok(NULL, " "); // Grab the text number argument after the space
        if (arg != NULL){
            uint8_t value = (uint8_t)atoi(arg); // Convert string to a real 8-bit integer
            memory[address] = OP_LD_A_IMMD;     // Write opcode to current slot
            memory[address + 1] = value;        // Write value to the slot right next to it
            return 2;                           // Tell the OS we just used up 2 bytes of RAM
        }
    }
    else if(strcmp(mnemonic, "LDB") == 0) {
        char *arg = strtok(NULL, " ");
        if (arg != NULL) {
            uint8_t value = (uint8_t)atoi(arg);
            memory[address] = OP_LD_B_IMMD;
            memory[address + 1] = value;
            return 2;
        }
    }
    else if (strcmp(mnemonic, "ADD") == 0) {
        memory[address] = OP_ADD_B_A;
        return 1; // Pure math calls don't need parameters; they only consume 1 byte of RAM
    }
    else if (strcmp(mnemonic, "SUB") == 0) {
        memory[address] = OP_SUB_B_A;
        return 1;
    }
    else if (strcmp(mnemonic, "MUL") == 0) {
        memory[address] = OP_MUL_B_A;
        return 1;
    }
    else if (strcmp(mnemonic, "JMP") == 0) {
        char *arg = strtok(NULL, " ");
        if (arg != NULL) {
            uint8_t target = (uint8_t)atoi(arg);
            memory[address] = OP_JMP;
            memory[address + 1] = target;
            return 2;
        }
    }
    else if (strcmp(mnemonic, "JNZ") == 0) {
        char *arg = strtok(NULL, " ");
        if (arg != NULL) {
            uint8_t target = (uint8_t)atoi(arg);
            memory[address] = OP_JNZ;
            memory[address + 1] = target;
            return 2;
        }
    }
    else if (strcmp(mnemonic, "STA") == 0) {
        char *arg = strtok(NULL, " ");
        if (arg != NULL) {
            uint8_t target = (uint8_t)atoi(arg);
            memory[address] = OP_STA_ADDR;
            memory[address + 1] = target;
            return 2;
        }
    }
    else if (strcmp(mnemonic, "STB") == 0) {
        char *arg = strtok(NULL, " ");
        if (arg != NULL) {
            uint8_t target = (uint8_t)atoi(arg);
            memory[address] = OP_STB_ADDR;
            memory[address + 1] = target;
            return 2;
        }
    }
    
    // Typo protection: prints out if you try to type an assembly command that doesn't exist
    printf("[ASSEMBLER ERROR] Unknown instruction: %s\n", mnemonic);
    return 0;
}

// Operating System Kernel Loop
int main() {
    Aethel8_CPU my_cpu;
    char input_buffer[100];
    uint8_t current_address = 0; // Tracks our position for sequential program assembly

    reset_system(&my_cpu);

    printf("\n=============================================\n");
    printf("   Welcome to Aethel-8 Toy OS v0.1-Alpha     \n");
    printf("   Type 'EXIT' to shut down the system.      \n");
    printf("=============================================\n\n");

    while (1)
    {
        printf("aethel8> ");

        // Grab user line input securely
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            break;
        }

        // Clean off the trailing newline character added by pressing enter
        input_buffer[strcspn(input_buffer, "\n")] = '\0';

        // Check for OS system dashboard tools before defaulting to the assembler
        if (strcmp(input_buffer, "EXIT") == 0) {
            printf("[SYSTEM] Shutting down Aethel-8 Virtual Machine. Goodbye.\n");
            break;
        }
        else if (strcmp(input_buffer, "RESET") == 0) {
            reset_system(&my_cpu);
            current_address = 0;
            continue;
        }
        else if (strcmp(input_buffer, "RUN") == 0) {
            printf("\n--- EXECUTE FROM PC (0x%02X) ---\n", my_cpu.PC);
            execute_cycle(&my_cpu);
            printf("\n");
            continue;
        }
        else if (strcmp(input_buffer, "RUNALL") == 0) {
            printf("\n-- AUTOMATED CPU RUN --\n");
            // Run automatically cycle-by-cycle until the next upcoming instruction is a HALT
            while (memory[my_cpu.PC] != OP_HALT) {
                execute_cycle(&my_cpu);
            }
            execute_cycle(&my_cpu); // Run the final HALT opcode to stop cleanly
            printf("\n");
            continue;
        }
        else if (strcmp(input_buffer, "DUMP") == 0) {
            // Visualizer tool: prints the entire 256-byte RAM space in a clean 16x16 grid
            printf("\n--- SYSTEM RAM DUMP (256 BYTES) ---\n");
            printf("    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
            printf("---------------------------------------------------\n");

            for (int r = 0; r < 16; r++) {
                printf("%02X | ", r * 16); // Row memory base index
                for (int c = 0; c < 16; c++) {
                    printf("%02X ", memory[r * 16 + c]);
                }
                printf("\n");
            }
            printf("\n");
            continue;
        }
        else if (strncmp(input_buffer, "PEEK", 4) == 0) {
            // Hardware debugging tool: spy on any memory box directly without running code
            char *arg = input_buffer + 4;
            int addr = atoi(arg);

            if (addr >= 0 && addr < MEMORY_SIZE) {
                printf("[OS RAM] Addr 0x%02X : 0x%02X (Decimal: %d)\n", addr, memory[addr], memory[addr]);
            } else {
                printf("[OS ERROR] Address out of 8-bit bounds (0-255).\n");
            }

            continue;
        }
        else if (strncmp(input_buffer, "POKE", 4) == 0) {
            // Hardware override tool: force-inject an arbitrary raw byte directly into a RAM address
            strtok(input_buffer, " "); // Skip the word "POKE"
            char *addr_str = strtok(NULL, " ");     // Grab target address
            char *val_str = strtok(NULL, " ");      // Grab byte value


            if (addr_str != NULL && val_str != NULL) {
                int addr = atoi(addr_str);
                int val = atoi(val_str);

                if (addr >= 0 && addr < MEMORY_SIZE && val >= 0 && val <= 255){
                    memory[addr] = (uint8_t)val;
                    printf("[OS RAM] POKED Addr 0x%02X with value 0x%02X\n", addr, val);
                } else {
                    printf("[OS ERROR] Invalid Address (0-255) or Value (0-255).\n");
                }
            } else {
                printf("[OS ERROR] Usage: POKE <address> <value>\n");
            }

            continue;
        }

        // If the line wasn't an OS dashboard command, pass it to the assembler to compile it into RAM
        int bytes_written = assemble_line(input_buffer, current_address);
        if (bytes_written > 0) {
            printf("[OS] Assembled successfully. %d bytes written at 0x%02X\n", bytes_written, current_address);
            current_address += bytes_written; // Shift pointer ahead so the next line doesn't overwrite this one
        }
    } 

    return 0;
}
