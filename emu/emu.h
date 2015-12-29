//
//  emu.h
//  emu
//
//  Created by Marc Bontje on 28-12-15.
//  Copyright © 2015 Marc Bontje. All rights reserved.
//

typedef struct flags {
    unsigned int c  : 1;
    unsigned int v1 : 1;
    unsigned int p  : 1;
    unsigned int v2 : 1;
    unsigned int h  : 1;
    unsigned int i  : 1;
    unsigned int z  : 1;
    unsigned int s  : 1;
} flags;

typedef struct registers {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    
    uint16_t sp;
    uint16_t pc;
} registers;

typedef struct emuState {
    registers *registers;
    flags     *flags;
    uint8_t   *memory;
} emuState;

typedef struct emuState emuState;

int loadFile(emuState *state, const char *filename, uint16_t offset);

void init(emuState *state);
int execute(emuState *state);
