//
//  8080Machine.m
//  8080
//
//  Created by Marc Bontje on 28-12-15.
//  Copyright © 2015 Marc Bontje. All rights reserved.
//

#import "8080Machine.h"

@implementation _080Machine

-(id) init
{
    memory = malloc(16 * 0x1000);
    
    int i, j;

    for (i=0; i< 224; i++)
    {
        for (j = 0; j < 256; j+= 8)
        {
            memory[0x2400 + (i*(256/8)) + j/8] = 0xFF;
        }
    }

    
    return self;
}

- (void) startEmulation
{
    timer = [NSTimer scheduledTimerWithTimeInterval: 0.001
                                                     target: self
                                                   selector:@selector(cycle)
                                                   userInfo: nil repeats:YES];
}

-(void) cycle
{
    NSLog(@"Cycle!");
}

-(void *) framebuffer
{
    return (void*) &memory[0x2400];
}

@end
