//
//  8080Machine.h
//  8080
//
//  Created by Marc Bontje on 28-12-15.
//  Copyright © 2015 Marc Bontje. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "emu.h"

@interface _080Machine : NSObject
{
    emuState *state;
    NSTimer  *timer;
    
    double_t now;
    double_t last;
}

-(void) loadFile:(NSString *)filename offset:(uint16_t)offset;
-(id) init;
-(void) start;
-(void) synchronize;
-(void *) framebuffer;

@end

