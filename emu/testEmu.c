//
//  testEmu.c
//  emu
//
//  Created by Marc Bontje on 28-12-15.
//  Copyright © 2015 Marc Bontje. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "emu.h"
#include "testEmu.h"

emuState *prepareState(int8_t opcode);

void testEmu() {
    printf("Testing emu...\n");
    testInit();
    testCycle();
}

void testInit() {
    printf(" * Testing init(): ");
    emuState *state = malloc(sizeof(emuState));
    init(state);

    assert(state->registers->a == 0);
    assert(state->registers->b == 0);
    assert(state->registers->c == 0);
    assert(state->registers->d == 0);
    assert(state->registers->e == 0);
    assert(state->registers->h == 0);
    assert(state->registers->l == 0);

    assert(state->sp == 0);
    assert(state->pc == 0);

    assert(state->flags->c == 0);
    assert(state->flags->p == 0);
    assert(state->flags->h == 0);
    assert(state->flags->i == 0);
    assert(state->flags->z == 0);
    assert(state->flags->s == 0);

    printf("OK!\n");
}

void testCycle() {
    printf(" * Testing cycle(): ");

    assert(cycle(prepareState(0x00)) == 1);
    
    printf("OK!\n");
}

emuState *prepareState(int8_t opcode) {
    emuState *state = malloc(sizeof(emuState));
    init(state);
    
    state->memory[0] = opcode;
    
    return state;
}
