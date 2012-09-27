//
//  GLView.m
//  Exercise2-HelloCone
//
//  Created by su_zhaoyang on 12-9-26.
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
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*)super.layer;
        eaglLayer.opaque = YES;
        EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
        m_context = [[EAGLContext alloc]initWithAPI:api];
        if (!m_context || ForceES1)
        {
            api = kEAGLRenderingAPIOpenGLES1;
            m_context = [[EAGLContext alloc]initWithAPI:api];
        }
        if(!m_context || ![EAGLContext setCurrentContext:m_context])
        {
            [self release];
            return nil;
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
        m_renderingEngine->Initialize(CGRectGetWidth(frame), CGRectGetHeight(frame));
        [self drawView:nil];
        m_timestamp = CACurrentMediaTime();
        CADisplayLink* displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [[UIDevice currentDevice]beginGeneratingDeviceOrientationNotifications];
        [[NSNotificationCenter defaultCenter]addObserver:self
                                                selector:@selector(didRotate:) name:UIDeviceOrientationDidChangeNotification object:nil];
        
        
    }
    return self;
}


-(void)drawView:(CADisplayLink *)displayLink
{
    if (displayLink)
    {
        float elapsedSecond = displayLink.timestamp - m_timestamp;
        m_timestamp = displayLink.timestamp;
        m_renderingEngine->UpdateAnimation(elapsedSecond);
    }
    m_renderingEngine->Render();
    [m_context presentRenderbuffer:GL_RENDERBUFFER];
}


-(void)didRotate:(NSNotification *)notification
{
    UIDeviceOrientation orientation = [[UIDevice currentDevice]orientation];
    m_renderingEngine->OnRotate((DeviceOrientation)orientation);
    [self drawView:nil];
}



@end
