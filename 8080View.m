//
//  8080View.m
//  8080
//
//  Created by Marc Bontje on 28-12-15.
//  Copyright © 2015 Marc Bontje. All rights reserved.
//

#import "8080View.h"

#define RGB_ON   0xFFFFFFFFL
#define RGB_OFF 0x00000000L

@implementation _080View

- (id)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self)
    {
        machine = [[_080Machine alloc] init];
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        buffer8888 = malloc(4 * 224*256);
        bitmapCtx = CGBitmapContextCreate(buffer8888, 224, 256, 8, 224*4, colorSpace, kCGImageAlphaNoneSkipFirst);
        
        //a 16ms time interval to get 60 fps
        renderTimer = [NSTimer timerWithTimeInterval:0.016
                                              target:self
                                            selector:@selector(timerFired:)
                                            userInfo:nil
                                             repeats:YES];
        
        [[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSDefaultRunLoopMode];
        [machine startEmulation];
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    int i, j;
    
    //Translate the 1-bit space invaders frame buffer into
    // my 32bpp RGB bitmap.  We have to rotate and
    // flip the image as we go.
    //
    unsigned char *b = (unsigned char *)buffer8888;
    unsigned char *fb = [machine framebuffer];
    for (i=0; i< 224; i++)
    {
        for (j = 0; j < 256; j+= 8)
        {
            int p;
            //Read the first 1-bit pixel
            // divide by 8 because there are 8 pixels
            // in a byte
            unsigned char pix = fb[(i*(256/8)) + j/8];
            
            //That makes 8 output vertical pixels
            // we need to do a vertical flip
            // so j needs to start at the last line
            // and advance backward through the buffer
            int offset = (255-j)*(224*4) + (i*4);
            unsigned int*p1 = (unsigned int*)(&b[offset]);
            for (p=0; p<8; p++)
            {
                if ( 0!= (pix & (1<<p)))
                    *p1 = RGB_ON;
                else
                    *p1 = RGB_OFF;
                p1-=224;  //next line
            }
        }
    }
    
    
    CGContextRef myContext = [[NSGraphicsContext currentContext] graphicsPort];
    CGImageRef ir = CGBitmapContextCreateImage(bitmapCtx);
    CGContextDrawImage(myContext, self.bounds, ir);
    CGImageRelease(ir);
}

// Timer callback method
- (void)timerFired:(id)sender
{
    [self setNeedsDisplay:YES];
}

@end
