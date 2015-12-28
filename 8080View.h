//
//  8080View.h
//  8080
//
//  Created by Marc Bontje on 28-12-15.
//  Copyright © 2015 Marc Bontje. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "8080Machine.h"

@interface _080View : NSView
{
    NSTimer     *renderTimer;
    _080Machine *machine;
    
    CGContextRef  bitmapCtx;
    unsigned char *buffer8888;
}

- (void)timerFired:(id)sender;

@end
