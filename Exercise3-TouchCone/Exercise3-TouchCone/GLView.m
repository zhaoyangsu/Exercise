//
//  GLView.m
//  Exercise3-TouchCone
//
//  Created by Hunk on 12-9-27.
//  Copyright (c) 2012年 su_zhaoyang. All rights reserved.
//

#import "GLView.h"

static BOOL ForceES1 = NO;

@implementation GLView

+(Class)layerClass
{
    return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*) super.layer;
        eaglLayer.opaque = YES;
        EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
        m_context = [[EAGLContext alloc]initWithAPI:api];
        if (!m_context || ForceES1)
        {
            api = kEAGLRenderingAPIOpenGLES1;
            m_context = [[EAGLContext alloc]initWithAPI:api];
        }
        if (!m_context || ![EAGLContext setCurrentContext:m_context])
        {
            [self release];
            return  nil;
        }
        
        if (api == kEAGLRenderingAPIOpenGLES1)
        {
            m_renderingEngine = CreateRender1();
        }
        else
        {
            m_renderingEngine = CreateRender2();
        }
        
        [m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        m_renderingEngine->Initialize(frame.size.width, frame.size.height);
        [self drawView:nil];
        m_timestamp = CACurrentMediaTime();
        CADisplayLink * displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    return self;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:self];
    m_renderingEngine->OnFingerDown(ivec2(location.x,location.y));
}
                                    
                                    
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:self];
    m_renderingEngine->OnFingerUp(ivec2(location.x,location.y));
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint oldLocation = [touch previousLocationInView:self];
    CGPoint currentLocation = [touch locationInView:self];
    m_renderingEngine->OnFingerMove(ivec2(oldLocation.x,oldLocation.y), ivec2(currentLocation.x,currentLocation.y));
}


@end
