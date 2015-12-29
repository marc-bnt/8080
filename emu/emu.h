//
//  emu.h
//  emu
//
//  Created by Marc Bontje on 28-12-15.
//  Copyright © 2015 Marc Bontje. All rights reserved.
//

typedef struct flags {
    unsigned int c  : 1;    // Carry
    unsigned int v1 : 1;    // Vacant bit
    unsigned int p  : 1;    // Parity
    unsigned int v2 : 1;    // Vacant bit
    unsigned int h  : 1;    // Auxilary carry
    unsigned int i  : 1;    // Interrupt
    unsigned int z  : 1;    // Zero
    unsigned int s  : 1;    // Sign
} flags;

typedef struct registers {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
} registers;

typedef struct emuState {
    registers *registers;
    flags     flags;
    uint8_t   *memory;
    
    uint16_t sp;
    uint16_t pc;
} emuState;

void init(emuState *state);

int cycle(emuState *state);
