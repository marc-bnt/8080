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
static void testReturn();
static void testIncrementAndDecrement();
static void testAdd();
static void testLogical();
static void testInputOutput();
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

    assert(state->flags.c == 0);
    assert(state->flags.p == 0);
    assert(state->flags.h == 0);
    assert(state->flags.i == 0);
    assert(state->flags.z == 0);
    assert(state->flags.s == 0);

    printf("OK!\n");
}

void testCycle() {
    printf(" * Testing cycle(): ");
    testMoveLoadAndStore();
    testStackOps();
    testJump();
    testCall();
    testReturn();
    testIncrementAndDecrement();
    testAdd();
    testLogical();
    testInputOutput();
    testControl();
    printf("OK!\n");
}

static void testMoveLoadAndStore() {
    emuState *state;

    // MOV A,D
    state = prepareState(0x7a, 0);
    state->registers->d = 0xab;
    
    assert(cycle(state) == 5);
    assert(state->registers->a == 0xab);
    assert(state->pc == 0x0001);
    
    // MOV A,H
    state = prepareState(0x7c, 0);
    state->registers->h = 0xab;

    assert(cycle(state) == 5);
    assert(state->registers->a == 0xab);
    assert(state->pc == 0x0001);
    
    // MOV L,A
    state = prepareState(0x6f, 0);
    state->registers->a = 0xab;
    
    assert(cycle(state) == 5);
    assert(state->registers->l == 0xab);
    assert(state->pc == 0x0001);
    
    // MOV M,A
    state = prepareState(0x77, 0);
    state->registers->a = 0x12;
    state->registers->h = 0xab;
    state->registers->l = 0xcd;

    assert(cycle(state) == 7);
    assert(state->memory[0xabcd] == 0x12);
    assert(state->pc == 0x0001);

    // MOV A,M
    state = prepareState(0x7e, 0);
    state->registers->h = 0xab;
    state->registers->l = 0xcd;
    state->memory[0xabcd] = 0xff;
    
    assert(cycle(state) == 7);
    assert(state->registers->a == 0xff);
    assert(state->pc == 0x0001);
    
    // MOV D,M
    state = prepareState(0x56, 0);
    state->registers->h = 0xab;
    state->registers->l = 0xcd;
    state->memory[0xabcd] = 0xff;
    
    assert(cycle(state) == 7);
    assert(state->registers->d == 0xff);
    assert(state->pc == 0x0001);
    
    // MOV E,M
    state = prepareState(0x5e, 0);
    state->registers->h = 0xab;
    state->registers->l = 0xcd;
    state->memory[0xabcd] = 0xff;
    
    assert(cycle(state) == 7);
    assert(state->registers->e == 0xff);
    assert(state->pc == 0x0001);

    // MOV H,M
    state = prepareState(0x66, 0);
    state->registers->h = 0xab;
    state->registers->l = 0xcd;
    state->memory[0xabcd] = 0xff;
    
    assert(cycle(state) == 7);
    assert(state->registers->h == 0xff);
    assert(state->pc == 0x0001);
    
    // MVI B
    state = prepareState(0x06, 0);
    state->memory[1] = 0xab;

    assert(cycle(state) == 7);
    assert(state->registers->b == 0xab);
    assert(state->pc == 0x0002);
    
    // MVI C
    state = prepareState(0x0e, 0);
    state->memory[1] = 0xab;
    
    assert(cycle(state) == 7);
    assert(state->registers->c == 0xab);
    assert(state->pc == 0x0002);
    
    // MVI H
    state = prepareState(0x26, 0);
    state->memory[1] = 0xab;
    
    assert(cycle(state) == 7);
    assert(state->registers->h == 0xab);
    assert(state->pc == 0x0002);
    
    // MVI M,H
    state = prepareState(0x36, 0);
    state->memory[1] = 0xab;
    state->registers->h = 0xb1;
    state->registers->l = 0x00;
    
    assert(cycle(state) == 10);
    assert(state->memory[0xb100] == 0xab);
    assert(state->pc == 0x0002);
    
    // LXI B
    state = prepareState(0x01, 0);
    state->memory[1] = 0x00;
    state->memory[2] = 0xB1;
    
    assert(cycle(state) == 10);
    assert(state->registers->b == 0xb1);
    assert(state->registers->c == 0x00);
    assert(state->pc == 0x0003);
    
    // LXI D
    state = prepareState(0x11, 0);
    state->memory[1] = 0x00;
    state->memory[2] = 0xB1;

    assert(cycle(state) == 10);
    assert(state->registers->d == 0xb1);
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

    // XCHG
    state = prepareState(0xeb, 0);
    state->registers->d = 0x03;
    state->registers->e = 0x04;
    state->registers->h = 0x01;
    state->registers->l = 0x02;
    
    assert(cycle(state) == 4);
    assert(state->registers->d == 0x01);
    assert(state->registers->e == 0x02);
    assert(state->registers->h == 0x03);
    assert(state->registers->l == 0x04);
    assert(state->pc == 0x0001);
}

static void testStackOps() {
    emuState *state;
    
    // PUSH B
    state = prepareState(0xc5, 0);
    state->registers->b = 0xab;
    state->registers->c = 0xcd;
    state->sp = 0x2400;
    
    assert(cycle(state) == 11);
    assert(state->memory[0x23ff] == 0xab);
    assert(state->memory[0x23fe] == 0xcd);
    assert(state->sp == 0x23fe);
    assert(state->pc == 0x0001);
    
    // PUSH D
    state = prepareState(0xd5, 0);
    state->registers->d = 0xab;
    state->registers->e = 0xcd;
    state->sp = 0x2400;

    assert(cycle(state) == 11);
    assert(state->memory[0x23ff] == 0xab);
    assert(state->memory[0x23fe] == 0xcd);
    assert(state->sp == 0x23fe);
    assert(state->pc == 0x0001);

    // PUSH H
    state = prepareState(0xe5, 0);
    state->registers->h = 0xab;
    state->registers->l = 0xcd;
    state->sp = 0x2400;
    
    assert(cycle(state) == 11);
    assert(state->memory[0x23ff] == 0xab);
    assert(state->memory[0x23fe] == 0xcd);
    assert(state->sp == 0x23fe);
    assert(state->pc == 0x0001);
    
    // PUSH PSW
    state = prepareState(0xf5, 0);
    state->registers->a = 0xab;
    state->flags.c = 1;
    state->flags.p = 1;
    state->flags.h = 1;
    state->flags.z = 1;
    state->flags.s = 1;
    state->sp = 0x2400;

    assert(cycle(state) == 11);
    assert(state->memory[0x23ff] == 0xab);
    
    assert(state->memory[0x23fe] == 0b11010101);
    assert(state->pc == 0x0001);

//    typedef struct flags {
//        unsigned int c  : 1;    // Carry
//        unsigned int v1 : 1;    // Vacant bit
//        unsigned int p  : 1;    // Parity
//        unsigned int v2 : 1;    // Vacant bit
//        unsigned int h  : 1;    // Auxilary carry
//        unsigned int i  : 1;    // Interrupt
//        unsigned int z  : 1;    // Zero
//        unsigned int s  : 1;    // Sign
//    } flags;
    
    // POP B
    state = prepareState(0xc1, 0);
    state->memory[0x23fe] = 0xcd;
    state->memory[0x23ff] = 0xab;
    state->sp = 0x23fe;
    
    assert(cycle(state) == 10);
    assert(state->registers->b == 0xab);
    assert(state->registers->c == 0xcd);
    assert(state->sp == 0x2400);
    assert(state->pc == 0x0001);
    
    // POP D
    state = prepareState(0xd1, 0);
    state->memory[0x23fe] = 0xcd;
    state->memory[0x23ff] = 0xab;
    state->sp = 0x23fe;
    
    assert(cycle(state) == 10);
    assert(state->registers->d == 0xab);
    assert(state->registers->e == 0xcd);
    assert(state->sp == 0x2400);
    assert(state->pc == 0x0001);
    
    // POP H
    state = prepareState(0xe1, 0);
    state->memory[0x23fe] = 0xcd;
    state->memory[0x23ff] = 0xab;
    state->sp = 0x23fe;
    
    assert(cycle(state) == 10);
    assert(state->registers->h == 0xab);
    assert(state->registers->l == 0xcd);
    assert(state->sp == 0x2400);
    assert(state->pc == 0x0001);
    
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
    
    // JNZ B
    state = prepareState(0xc2, 0);
    state->registers->b = 0x01;
    state->memory[1] = 0x32;
    state->memory[2] = 0x1a;
    
    assert(cycle(state) == 10);
    assert(state->pc == 0x1a32);
    
    state = prepareState(0xc2, 0);
    state->registers->b = 0x00;
    cycle(state);
    
    assert(state->pc == 0x0003);
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

static void testReturn() {
    emuState *state;
    
    // RET
    state = prepareState(0xc9, 0);
    state->memory[0x23fe] = 0xdc;
    state->memory[0x23ff] = 0x18;
    state->sp = 0x23fe;
    
    assert(cycle(state) == 10);
    assert(state->pc == 0x18dc);
    assert(state->sp == 0x2400);
}

static void testIncrementAndDecrement() {
    emuState *state;
    
    // DCR B
    state = prepareState(0x05, 0);
    state->registers->b = 0x10;
    
    assert(cycle(state) == 5);
    assert(state->pc == 0x0001);
    assert(state->registers->b == 0x0f);
    assert(state->flags.z == 0);
    assert(state->flags.s == 0);
    assert(state->flags.p == 1);
    
    state = prepareState(0x05, 0);
    state->registers->b = 0xff;
    cycle(state);
    
    assert(state->registers->b == 0xfe);
    assert(state->flags.z == 0);
    assert(state->flags.s == 1);
    assert(state->flags.p == 0);
    
    state = prepareState(0x05, 0);
    state->registers->b = 0x01;
    cycle(state);
    
    assert(state->registers->b == 0x00);
    assert(state->flags.z == 1);
    assert(state->flags.s == 0);
    assert(state->flags.p == 1);

    // DCR C
    state = prepareState(0x0d, 0);
    state->registers->c = 0x10;
    
    assert(cycle(state) == 5);
    assert(state->pc == 0x0001);
    assert(state->registers->c == 0x0f);
    
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

static void testAdd() {
    emuState *state;

    // DAD B
    state = prepareState(0x09, 0);
    state->registers->b = 0x00;
    state->registers->c = 0x01;
    state->registers->h = 0xff;
    state->registers->l = 0xff;
    
    assert(cycle(state) == 10);
    assert(state->registers->h == 0x00);
    assert(state->registers->l == 0x00);
    assert(state->pc == 0x0001);
    assert(state->flags.c == 1);
    
    // DAD D
    state = prepareState(0x19, 0);
    state->registers->d = 0x00;
    state->registers->e = 0x01;
    state->registers->h = 0xff;
    state->registers->l = 0xff;

    assert(cycle(state) == 10);
    assert(state->registers->h == 0x00);
    assert(state->registers->l == 0x00);
    assert(state->pc == 0x0001);
    assert(state->flags.c == 1);
    
    // DAD H
    state = prepareState(0x29, 0);
    state->registers->h = 0x00;
    state->registers->l = 0xff;

    assert(cycle(state) == 10);
    assert(state->registers->h == 0x01);
    assert(state->registers->l == 0xfe);
    assert(state->pc == 0x0001);
    assert(state->flags.c == 0);
}

static void testLogical() {
    emuState *state;
    
    // CPI
    state = prepareState(0xfe, 0);
    state->memory[1] = 0x41;
    state->registers->a = 0x40;
    
    assert(cycle(state) == 7);
    assert(state->pc == 0x0002);
    assert(state->flags.z == 0);
    assert(state->flags.s == 1);
    assert(state->flags.p == 1);
    assert(state->flags.c == 1);
}

static void testInputOutput() {
    emuState *state;

    // OUT
    state = prepareState(0xd3, 0);
    
    assert(cycle(state) == 10);
    assert(state->pc == 0x0002);
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
