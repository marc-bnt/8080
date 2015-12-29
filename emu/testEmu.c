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

emuState *prepareState(int8_t opcode, int16_t offset);

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
    emuState *state;

    // NOP
    state = prepareState(0x00, 0);
    assert(cycle(state) == 4);
    assert(state->pc == 0x0001);

    // MVI B
    state = prepareState(0x06, 0);
    state->memory[1] = 0xAB;
    assert(cycle(state) == 7);
    assert(state->registers->b == 0xAB);
    
    // LXI SP
    state = prepareState(0x31, 0);
    state->memory[1] = 0x00;
    state->memory[2] = 0x24;
    assert(cycle(state) == 10);
    assert(state->pc == 0x0003);
    assert(state->sp == 0x2400);

    // JMP
    state = prepareState(0xc3, 0);
    state->memory[1] = 0xd4;
    state->memory[2] = 0x18;
    assert(cycle(state) == 10);
    assert(state->pc == 0x18d4);
    
    // CALL
    state = prepareState(0xcd, 0x18d9);
    state->memory[0x18d9 + 1] = 0xe6;
    state->memory[0x18d9 + 2] = 0x01;
    state->sp = 0x2400;
    assert(cycle(state) == 17);
    assert(state->memory[0x23ff] == 0x18);
    assert(state->memory[0x23fe] == 0xdc);
    assert(state->sp == 0x23fe);
    assert(state->pc == 0x01e6);

    printf("OK!\n");
}

emuState *prepareState(int8_t opcode, int16_t offset) {
    emuState *state = malloc(sizeof(emuState));
    init(state);
    
    state->memory[offset] = opcode;
    state->pc = offset;
    
    return state;
}
