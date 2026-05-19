#include <stdint.h>

#define MEMORY_SIZE 256
#define OP_HALT 0x00
#define OP_LD_A_IMMD 0x01
#define OP_LD_B_IMMD 0x02
#define OP_ADD_B_A 0x03
#define OP_SUB_B_A 0x04
#define OP_MUL_B_A 0x05
#define OP_JMP 0x06
#define OP_JNZ 0x07
#define OP_STA_ADDR 0x08
#define OP_STB_ADDR 0x09

typedef struct{
    uint8_t A;
    uint8_t B;
    uint8_t PC;
    uint8_t ZF;
} Aethel8_CPU;

uint8_t memory[MEMORY_SIZE];
void reset_system(Aethel8_CPU *cpu);
void execute_cycle(Aethel8_CPU *cpu);