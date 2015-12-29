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
    int cycles = 1;
    
    // Fetch instruction at PC address
    uint8_t offset = state->pc++;
    uint8_t opcode = state->memory[offset];
    
#if LOG
    printf("0x%08x: 0x%02x\n", offset, opcode);
#endif
    
    switch (opcode) {
        case 0x00:  // NOP
            break;
            
        case 0xc3:  // NOP
            break;
            
        default:
            printf("Invalid opcode 0x%02x", opcode);
            exit(1);
    }
    
    return cycles;
}
