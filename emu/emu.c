//
//  emu.c
//  emu
//
//  Created by Marc Bontje on 28-12-15.
//  Copyright © 2015 Marc Bontje. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#import "emu.h"

#define LOG  0

void init(emuState *state) {
    state->registers = malloc(sizeof(registers));
    state->flags = malloc(sizeof(flags));

    state->sp = 0;
    state->pc = 0;
    
    state->memory = malloc(16 * 0x1000);
}

int cycle(emuState *state) {
    int cycles = -1;
    
    // Fetch instruction at PC address
    uint16_t offset = state->pc++;
    uint8_t *opcode = &state->memory[offset];
    
#if LOG
    printf("0x%08x: 0x%02x (0x%02x, 0x%02x) \n", offset, *opcode, opcode[1], opcode[2]);
#endif
    
    switch (*opcode) {
        case 0x00:  // NOP
            cycles = 4;
            break;
            
        case 0x06:  // MVI B
            cycles = 7;
            state->registers->b = opcode[1];
            state->pc++;
            break;
            
        case 0x31:  // LXI SP
            cycles = 10;
            state->sp = opcode[2] << 8 | opcode[1];
            state->pc += 2;
            break;
            
        case 0xc3:  // JMP
            cycles = 10;
            state->pc = opcode[2] << 8 | opcode[1];
            break;
            
        case 0xcd: {    // CALL
            int16_t ret = state->pc + 2;
            cycles = 17;

            state->memory[state->sp - 1] = ret >> 8;
            state->memory[state->sp - 2] = ret;

            state->sp -= 2;
            state->pc = opcode[2] << 8 | opcode[1];
            break;
        }

        default:
            printf("Invalid opcode 0x%02x", *opcode);
            exit(1);
    }
    
    return cycles;
}
