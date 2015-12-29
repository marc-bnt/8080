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

static void testMoveLoadAndStore();
static void testStackOps();
static void testJump();
static void testCall();
static void testIncrementAndDecrement();
static void testControl();

static emuState *prepareState(int8_t opcode, int16_t offset);

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
    testMoveLoadAndStore();
    testStackOps();
    testJump();
    testCall();
    testIncrementAndDecrement();
    testControl();
    printf("OK!\n");
}

static void testMoveLoadAndStore() {
    emuState *state;
    
    // MOV M,A
    state = prepareState(0x77, 0);
    state->registers->a = 0x12;
    state->registers->h = 0xab;
    state->registers->l = 0xcd;

    assert(cycle(state) == 7);
    assert(state->memory[0xabcd] == 0x12);
    assert(state->pc == 0x0001);

    // MVI B
    state = prepareState(0x06, 0);
    state->memory[1] = 0xAB;

    assert(cycle(state) == 7);
    assert(state->registers->b == 0xAB);
    assert(state->pc == 0x0002);
    
    // LXI D
    state = prepareState(0x11, 0);
    state->memory[1] = 0x00;
    state->memory[2] = 0xB1;

    assert(cycle(state) == 10);
    assert(state->registers->d == 0xB1);
    assert(state->registers->e == 0x00);
    assert(state->pc == 0x0003);
    
    // LXI H
    state = prepareState(0x21, 0);
    state->memory[1] = 0x00;
    state->memory[2] = 0x20;
    
    assert(cycle(state) == 10);
    assert(state->registers->h == 0x20);
    assert(state->registers->l == 0x00);
    assert(state->pc == 0x0003);

    // LDAX D
    state = prepareState(0x1a, 0);
    state->memory[0xabcd] = 0x12;
    state->registers->d = 0xab;
    state->registers->e = 0xcd;

    assert(cycle(state) == 7);
    assert(state->registers->a == 0x12);
    assert(state->pc == 0x0001);
}

static void testStackOps() {
    emuState *state;
    
    // LXI SP
    state = prepareState(0x31, 0);
    state->memory[1] = 0x00;
    state->memory[2] = 0x24;

    assert(cycle(state) == 10);
    assert(state->pc == 0x0003);
    assert(state->sp == 0x2400);
}

static void testJump() {
    emuState *state;
    
    // JMP
    state = prepareState(0xc3, 0);
    state->memory[1] = 0xd4;
    state->memory[2] = 0x18;

    assert(cycle(state) == 10);
    assert(state->pc == 0x18d4);
}

static void testCall() {
    emuState *state;
    
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
}

static void testIncrementAndDecrement() {
    emuState *state;
    
    // DCR B
    state = prepareState(0x05, 0);
    state->registers->b = 0x10;
    
    assert(cycle(state) == 5);
    assert(state->pc == 0x0001);
    assert(state->registers->b == 0x0f);
    assert(state->flags->z == 0);
    assert(state->flags->s == 0);
    assert(state->flags->p == 1);
    
    state = prepareState(0x05, 0);
    state->registers->b = 0xff;
    cycle(state);
    
    assert(state->registers->b == 0xfe);
    assert(state->flags->z == 0);
    assert(state->flags->s == 1);
    assert(state->flags->p == 0);
    
    state = prepareState(0x05, 0);
    state->registers->b = 0x01;
    cycle(state);
    
    assert(state->registers->b == 0x00);
    assert(state->flags->z == 1);
    assert(state->flags->s == 0);
    assert(state->flags->p == 1);

    // INX D
    state = prepareState(0x13, 0);
    state->registers->d = 0x0f;
    state->registers->e = 0xff;

    assert(cycle(state) == 5);
    assert(state->registers->d == 0x10);
    assert(state->registers->e == 0x00);
    assert(state->pc == 0x0001);

    // INX H
    state = prepareState(0x23, 0);
    state->registers->h = 0x0f;
    state->registers->l = 0xff;

    assert(cycle(state) == 5);
    assert(state->registers->h == 0x10);
    assert(state->registers->l == 0x00);
    assert(state->pc == 0x0001);
}

static void testControl() {
    emuState *state;
    
    // NOP
    state = prepareState(0x00, 0);

    assert(cycle(state) == 4);
    assert(state->pc == 0x0001);
}

emuState *prepareState(int8_t opcode, int16_t offset) {
    emuState *state = malloc(sizeof(emuState));
    init(state);
    
    state->memory[offset] = opcode;
    state->pc = offset;
    
    return state;
}
