#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MEM_SIZE 256
#define NUM_REGISTERS 8

#define LOAD 0x1
#define ADD 0x2
#define SUB 0x3
#define STORE 0x4
#define AND 0x5
#define OR 0x6
#define JMP 0x7
#define HALT 0xF

uint16_t memory[MEM_SIZE];
uint16_t registers[NUM_REGISTERS];
uint16_t PC = 0;
bool running = true;

uint16_t ALU(uint16_t a, uint16_t b, uint8_t op)
{
    switch (op)
    {
    case ADD:
        return a + b;
    case SUB:
        return a - b;
    case AND:
        return a & b;
    case OR:
        return a | b;
    default:
        return 0;
    }
}

// This function simulates the control logic for each instruction
void execute_instruction(uint16_t instr)
{
    uint8_t opcode = (instr >> 8) & 0xF;
    uint8_t operand = instr & 0xFF;

    printf("Fetch instruction: 0x%03X (PC=%d)\n", instr, PC);

    switch (opcode)
    {
    case LOAD:
    {
        // Note: Here, ı use the lower 3 bits to choose a register (0-7)
        uint8_t reg = (operand >> 5) & 0x7;
        uint8_t addr = operand & 0x1F;
        registers[reg] = memory[addr];
        printf("LOAD R%d <= MEM[%d] = %d\n", reg, addr, memory[addr]);
        break;
    }
    case STORE:
    {
        uint8_t reg = (operand >> 5) & 0x7;
        uint8_t addr = operand & 0x1F;
        memory[addr] = registers[reg];
        printf("STORE MEM[%d] <= R%d = %d\n", addr, reg, registers[reg]);
        break;
    }
    case ADD:
    case SUB:
    case AND:
    case OR:
    {
        // ALU operations use two source registers and store result in a third one
        uint8_t reg1 = (operand >> 5) & 0x7;
        uint8_t reg2 = (operand >> 2) & 0x7;
        uint8_t dest = operand & 0x3;
        registers[dest] = ALU(registers[reg1], registers[reg2], opcode);
        const char *opname = (opcode == ADD) ? "ADD" : (opcode == SUB) ? "SUB"
                                                   : (opcode == AND)   ? "AND"
                                                                       : "OR";
        printf("%s R%d = R%d %s R%d = %d\n", opname, dest, reg1, opname, reg2, registers[dest]);
        break;
    }
    case JMP:
    {
        // Unconditional jump to address
        PC = operand;
        printf("JMP to %d\n", operand);
        return;
    }
    case HALT:
    {
        printf("HALT encountered. Stopping execution.\n");
        running = false;
        break;
    }
    default:
        printf("Unknown opcode: %X\n", opcode);
        running = false;
        break;
    }

    PC++;
}

void load_program()
{
    // This is my test program that shows all the instructions working.
    // Initialize registers and memory
    memory[0] = 15;
    memory[1] = 5;
    memory[2] = 12;
    memory[3] = 8;

    memory[0] = (LOAD << 8) | (0 << 5) | 0;
    memory[1] = (LOAD << 8) | (1 << 5) | 1;
    memory[2] = (ADD << 8) | (0 << 5) | (1 << 2) | 2;
    memory[3] = (SUB << 8) | (0 << 5) | (1 << 2) | 3;
    memory[4] = (LOAD << 8) | (2 << 5) | 2;
    memory[5] = (LOAD << 8) | (3 << 5) | 3;
    memory[6] = (AND << 8) | (4 << 5) | (5 << 2) | 6;
    memory[7] = (OR << 8) | (4 << 5) | (5 << 2) | 7;
    memory[8] = (STORE << 8) | (2 << 5) | 10;
    memory[9] = (JMP << 8) | 11;
    memory[11] = (HALT << 8);
}

int main()
{
    printf("==== Microprogrammed Control Unit Simulator ====\n\n");

    load_program();

    while (running)
    {
        execute_instruction(memory[PC]);
    }

    return 0;
}
