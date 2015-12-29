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
#include "emu.h"

void init(emuState *state) {
    state->registers = malloc(sizeof(registers));
    state->flags = malloc(sizeof(flags));

    state->registers->sp = 0;
    state->registers->pc = 0;
    
    state->memory = malloc(16 * 0x1000);
}

int execute(emuState *state) {
    int cycles = 1;
    
    return cycles;
}
