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

static uint8_t getParity(uint8_t value) {
    int i;
    int p = 0;
    
    value = (value & ((1 << 8) - 1));
    
    for (i = 0; i < 8; i++) {
        if (value & 0x1) {
            p++;
        }
        
        value = value >> 1;
    }

    return (0 == (p & 0x1));
}

static void setFlags(emuState *state, uint8_t value) {
    state->flags->z = (value == 0);
    state->flags->s = ((value & 0x80) == 0x80);
    state->flags->p = getParity(value);
}

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
    
    // TODO: Disable writing to ROM
    
    switch (*opcode) {
        case 0x00:  // NOP
            cycles = 4;
            break;
            
        case 0x05:  // DCR B
            cycles = 5;
            setFlags(state, --state->registers->b);
            
            break;
            
        case 0x06:  // MVI B
            cycles = 7;
            state->registers->b = opcode[1];
            state->pc++;
            break;

        case 0x11:  // LXI D
            cycles = 10;
            state->registers->d = opcode[2];
            state->registers->e = opcode[1];
            state->pc += 2;
            break;
            
        case 0x13: {    // INX D
            int16_t result = (state->registers->d << 8 | state->registers->e) + 1;
            cycles = 5;
            state->registers->d = result >> 8;
            state->registers->e = result;
            break;
        }

        case 0x1a:  // LDAX D
            cycles = 7;
            state->registers->a = state->memory[state->registers->d << 8 | state->registers->e];
            break;
        
        case 0x21:  // LXI H
            cycles = 10;
            state->registers->h = opcode[2];
            state->registers->l = opcode[1];
            state->pc += 2;
            break;
            
        case 0x23: {    // INX H
            int16_t result = (state->registers->h << 8 | state->registers->l) + 1;
            cycles = 5;
            state->registers->h = result >> 8;
            state->registers->l = result;
            break;
        }
            
        case 0x31:  // LXI SP
            cycles = 10;
            state->sp = opcode[2] << 8 | opcode[1];
            state->pc += 2;
            break;
            
        case 0x77:  // MOV M,A
            cycles = 7;
            state->memory[state->registers->h << 8 | state->registers->l] = state->registers->a;
            break;
            
        case 0xc2:  // JNZ B
            cycles = 10;

            if (state->registers->b != 0)
                state->pc = opcode[2] << 8 | opcode[1];
            else
                state->pc += 2;
            
            break;
            
        case 0xc3:  // JMP
            cycles = 10;
            state->pc = opcode[2] << 8 | opcode[1];
            break;
            
        case 0xc9:  // RET
            cycles = 10;
            state->pc = state->memory[state->sp + 1] << 8 | state->memory[state->sp];
            state->sp += 2;
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
            printf("Invalid opcode: 0x%02x (0x%02x, 0x%02x) \n", *opcode, opcode[1], opcode[2]);
            exit(1);
    }
    
#if LOG
    printf("0x%08x: 0x%02x (0x%02x, 0x%02x) \n", offset, *opcode, opcode[1], opcode[2]);
#endif
    
    return cycles;
}
