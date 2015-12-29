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

#define TEST 1

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
#if TEST
    testEmu();
#endif
    
    state = malloc(sizeof(emuState));
    init(state);
    
    [self loadFile:@"invaders.h" offset:0x0000];
    [self loadFile:@"invaders.g" offset:0x0800];
    [self loadFile:@"invaders.f" offset:0x1000];
    [self loadFile:@"invaders.e" offset:0x1800];
    
    return self;
}

- (void) start
{
    timer = [NSTimer scheduledTimerWithTimeInterval: 0.001
                                             target: self
                                           selector: @selector(synchronize)
                                           userInfo: nil repeats:YES];
}

-(void) synchronize
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
        cycles += cycle(state);
    }
    
    last = now;
}

-(void *) framebuffer
{
    return (void*) &state->memory[0x2400];
}

@end
