//
//  8080Machine.m
//  8080
//
//  Created by Marc Bontje on 28-12-15.
//  Copyright © 2015 Marc Bontje. All rights reserved.
//

#include <sys/time.h>
#import "8080Machine.h"
#import "testEmu.h"

@implementation _080Machine

-(void)loadFile:(NSString *)filename offset:(uint16_t)offset
{
    NSBundle *mainbundle = [NSBundle mainBundle];
    NSString *filepath = [mainbundle pathForResource:filename ofType:NULL];
    
    NSData *data = [[NSData alloc] initWithContentsOfFile:filepath];
    
    memcpy(&state->memory[offset], [data bytes], [data length]);
}

-(id) init
{
    testEmu();
    
    state = malloc(sizeof(emuState));
    init(state);
    
    [self loadFile:@"invaders.h" offset:0x0000];
    [self loadFile:@"invaders.g" offset:0x0800];
    [self loadFile:@"invaders.f" offset:0x1000];
    [self loadFile:@"invaders.e" offset:0x1800];
    
//    int i, j;
//
//    for (i=0; i< 224; i++) {
//        for (j = 0; j < 256; j+= 8) {
//            state->memory[0x2400 + (i*(256/8)) + j/8] = 0xFF;
//        }
//    }

    return self;
}

- (void) startEmulation
{
    timer = [NSTimer scheduledTimerWithTimeInterval: 0.001
                                             target: self
                                           selector: @selector(cycle)
                                           userInfo: nil repeats:YES];
}

-(void) cycle
{
    struct timeval time;
    gettimeofday(&time, NULL);
    
    now = (double)time.tv_sec * 1e6 + (double)time.tv_usec;

    // Clock runs at 2 MHz
    //  => 500 ns/cycle (1/(2*10^6 Hz) = 5*10^-7 s)
    //  => 2 cycles/μs
    
    int cycles = 0;
    int requiredCycles = (now - last) * 2;
    
    while (cycles < requiredCycles) {
        cycles += execute(state);
    }
    
    NSLog(@"%d", cycles);
    
    last = now;
}

-(void *) framebuffer
{
    return (void*) &state->memory[0x2400];
}

@end
