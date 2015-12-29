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

#define LOG 0

int count = 0;

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
    state->flags->c = 0;
    state->flags->h = 0;
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
    
    switch (*opcode) {
        case 0x00:  // NOP
            cycles = 4;
            break;
            
        case 0x01:  // LXI B
            cycles = 10;
            state->registers->b = opcode[2];
            state->registers->c = opcode[1];
            state->pc += 2;
            break;
            
        case 0x03: {    // INX B
            int16_t result = (state->registers->b << 8 | state->registers->c) + 1;
            cycles = 5;
            state->registers->b = result >> 8;
            state->registers->c = result;
            break;
        }
            
        case 0x05:  // DCR B
            cycles = 5;
            setFlags(state, --state->registers->b);
            break;
            
        case 0x06:  // MVI B
            cycles = 7;
            state->registers->b = opcode[1];
            state->pc++;
            break;

        case 0x09: {    // DAD B
            cycles = 10;
            int32_t value = state->registers->h << 8 | state->registers->l;
            value += state->registers->b << 8 | state->registers->c;
            
            state->registers->h = value >> 8;
            state->registers->l = value;
            state->flags->c = (value > 0xffff);
            break;
        }
            
        case 0x0a:  // LDAX B
            cycles = 7;
            state->registers->a = state->memory[state->registers->b << 8 | state->registers->c];
            break;
            
        case 0x0d:  // DCR C
            cycles = 5;
            setFlags(state, --state->registers->c);
            break;
            
        case 0x0e:  // MVI C
            cycles = 7;
            state->registers->c = opcode[1];
            state->pc++;
            break;
            
        case 0x0f: {    // RRC
            int8_t a = state->registers->a;
            cycles = 4;
            
            state->registers->a = a << 1 | (a >> 1 & 1);
            state->flags->c = a;
            break;
        }
            
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

        case 0x19: {    // DAD D
            cycles = 10;
            int32_t value = state->registers->h << 8 | state->registers->l;
            value += state->registers->d << 8 | state->registers->e;
            
            state->registers->h = value >> 8;
            state->registers->l = value;
            state->flags->c = (value > 0xffff);
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
            
        case 0x26:  // MVI H
            cycles = 7;
            state->registers->h = opcode[1];
            state->pc++;
            break;
            
        case 0x29: {    // DAD H
            cycles = 10;
            int32_t value = state->registers->h << 8 | state->registers->l;
            value += value;
            
            state->registers->h = value >> 8;
            state->registers->l = value;
            state->flags->c = (value > 0xffff);
            break;
        }
            
        case 0x31:  // LXI SP
            cycles = 10;
            state->sp = opcode[2] << 8 | opcode[1];
            state->pc += 2;
            break;
            
        case 0x32:  // STA
            cycles = 13;
            state->memory[opcode[2] << 8 | opcode[1]] = state->registers->a;
            state->pc += 2;
            break;
            
        case 0x36:  // MVI M,H
            cycles = 10;
            state->memory[state->registers->h << 8 | state->registers->l] = opcode[1];
            state->pc++;
            break;
            
        case 0x37:  // STC M,H
            cycles = 4;
            state->flags->c = 1;
            break;

        case 0x3a:  // LDA
            cycles = 13;
            state->registers->a = state->memory[opcode[2] << 8 | opcode[1]];
            state->pc += 2;
            break;
            
        case 0x3d:  // DCR A
            cycles = 5;
            setFlags(state, --state->registers->a);
            break;
            
        case 0x3e:  // MVI A
            cycles = 7;
            state->registers->a = opcode[1];
            state->pc++;
            break;
            
        case 0x56:  // MOV D,M
            cycles = 7;
            state->registers->d = state->memory[state->registers->h << 8 | state->registers->l];
            break;
            
        case 0x57:  // MOV D,A
            cycles = 5;
            state->registers->d = state->registers->a;
            break;
            
        case 0x5e:  // MOV E,M
            cycles = 7;
            state->registers->e = state->memory[state->registers->h << 8 | state->registers->l];
            break;
            
        case 0x5f:  // MOV E,A
            cycles = 5;
            state->registers->e = state->registers->a;
            break;
            
        case 0x66:  // MOV H,M
            cycles = 7;
            state->registers->h = state->memory[state->registers->h << 8 | state->registers->l];
            break;
            
        case 0x67:  // MOV H,A
            cycles = 5;
            state->registers->h = state->registers->a;
            break;
            
        case 0x6f:  // MOV L,A
            cycles = 5;
            state->registers->l = state->registers->a;
            break;
            
        case 0x77:  // MOV M,A
            cycles = 7;
            state->memory[state->registers->h << 8 | state->registers->l] = state->registers->a;
            break;
            
        case 0x7a:  // MOV A,D
            cycles = 5;
            state->registers->a = state->registers->d;
            break;
            
        case 0x7b:  // MOV A,E
            cycles = 5;
            state->registers->a = state->registers->e;
            break;
            
        case 0x7c:  // MOV A,H
            cycles = 5;
            state->registers->a = state->registers->h;
            break;
            
        case 0x7e:  // MOV A,M
            cycles = 7;
            state->registers->a = state->memory[state->registers->h << 8 | state->registers->l];
            break;
            
        case 0xa7:  // ANA A
            cycles = 4;
            state->registers->a = state->registers->a & state->registers->a;
            setFlags(state, state->registers->a);
            break;
            
        case 0xaf:  // XRA A
            cycles = 4;
            state->registers->a = state->registers->a ^ state->registers->a;
            setFlags(state, state->registers->a);
            break;
            
        case 0xc1:  // POP B
            cycles = 10;
            state->registers->c = state->memory[state->sp];
            state->registers->b = state->memory[state->sp + 1];
            state->sp += 2;
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
            
        case 0xc5:  // PUSH B
            cycles = 11;
            state->memory[state->sp - 1] = state->registers->b;
            state->memory[state->sp - 2] = state->registers->c;
            state->sp -= 2;
            break;
            
        case 0xc6: {    // ADI
            int16_t value = (int16_t)state->registers->a + (int16_t)opcode[1];
            cycles = 7;
            
            state->registers->a = value;
            setFlags(state, state->registers->a);
            state->flags->c = (value > 0xff);
            state->pc++;
            break;
        }
            
        case 0xc8:  // RZ
            if (state->flags->z) {
                cycles = 11;
                state->pc = state->memory[state->sp + 1] << 8 | state->memory[state->sp];
                state->sp += 2;
            } else {
                cycles = 5;
            }
            
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
            
        case 0xd1:  // POP D
            cycles = 10;
            state->registers->e = state->memory[state->sp];
            state->registers->d = state->memory[state->sp + 1];
            state->sp += 2;
            break;

        case 0xd3:  // OUT
            cycles = 10;
            state->pc++;
            break;

        case 0xd5:  // PUSH D
            cycles = 11;
            state->memory[state->sp - 1] = state->registers->d;
            state->memory[state->sp - 2] = state->registers->e;
            state->sp -= 2;
            break;
            
        case 0xe5:  // PUSH H
            cycles = 11;
            state->memory[state->sp - 1] = state->registers->h;
            state->memory[state->sp - 2] = state->registers->l;
            state->sp -= 2;
            break;

        case 0xe6:  // ANI
            cycles = 7;
            state->registers->a = state->registers->a & opcode[1];
            setFlags(state, state->registers->a);
            state->pc++;
            break;
            
        case 0xe1:  // POP H
            cycles = 10;
            state->registers->l = state->memory[state->sp];
            state->registers->h = state->memory[state->sp + 1];
            state->sp += 2;
            break;
            
        case 0xeb: {    // XCHG
            uint8_t h = state->registers->h;
            uint8_t l = state->registers->l;

            cycles = 4;
            state->registers->h = state->registers->d;
            state->registers->l = state->registers->e;
            state->registers->d = h;
            state->registers->e = l;
            break;
        }
            
        case 0xf1:  // POP PSW
            cycles = 10;
            *(uint8_t *)state->flags = state->memory[state->sp];
            state->registers->a = state->memory[state->sp + 1];
            state->sp += 2;
            break;
            
        case 0xf5:  // PUSH PSW
            cycles = 11;
            state->memory[state->sp - 1] = state->registers->a;
            state->memory[state->sp - 2] = *(uint8_t *)state->flags;
            state->sp -= 2;
            break;
    
        case 0xfb:  // EI
            cycles = 4;
            state->flags->i = 1;
            break;
            
        case 0xfe:  // CPI
            cycles = 7;
            setFlags(state, state->registers->a - opcode[1]);
            state->flags->c = (state->registers->a < opcode[1]);
            state->pc++;
            
            break;

        default:
            printf("Invalid opcode at 0x%04x: 0x%02x (0x%02x, 0x%02x)\n", offset, *opcode, opcode[1], opcode[2]);
            printf("%d instructions executed succesfully\n", count);
            exit(1);
    }
    
    count++;

#if LOG
    printf("0x%04x: 0x%02x (0x%02x, 0x%02x) \n", offset, *opcode, opcode[1], opcode[2]);
#endif
    
    return cycles;
}
