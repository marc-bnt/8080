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

void testEmu() {
    printf("Testing emu...\n");
    testInit();
    testExecute();
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

    assert(state->registers->sp == 0);
    assert(state->registers->pc == 0);

    assert(state->flags->c == 0);
    assert(state->flags->p == 0);
    assert(state->flags->h == 0);
    assert(state->flags->i == 0);
    assert(state->flags->z == 0);
    assert(state->flags->s == 0);

    printf("OK!\n");
}

void testExecute() {
    printf(" * Testing execute(): ");
    emuState *state = malloc(sizeof(emuState));
    init(state);
    
    assert(execute(state) == 1);
    
    printf("OK!\n");
}
