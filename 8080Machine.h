//
//  8080Machine.h
//  8080
//
//  Created by Marc Bontje on 28-12-15.
//  Copyright © 2015 Marc Bontje. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface _080Machine : NSObject
{
    uint8_t	*memory;
    NSTimer *timer;
}

-(id) init;
-(void) startEmulation;
-(void) cycle;
-(void *) framebuffer;

@end

