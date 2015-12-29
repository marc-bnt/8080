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

int loadFile(emuState *state, const char *filename, uint16_t offset)
{
    int size = 0;
    uint8_t *result = &state->memory[offset];
    
    FILE *f = fopen(filename, "rb");
    
    if (f == NULL) {
        return -1;
    }
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (size != fread(result, sizeof(char), size, f)) {
        return -2;
    }
    
    fclose(f);

    return size;
}

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
