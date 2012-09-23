//
//  GLView.h
//  Exercise1
//
//  Created by xiu on 9/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGlES/EAGL.h>
#import <QuartzCore/QuartzCore.h>
#import "IRenderingEngine.hpp"

@interface GLView : UIView
{
@private
    EAGLContext* m_context;
    IRenderingEngine* m_renderingEngine;
    float m_timestamp;
}

-(void) drawView:(CADisplayLink*)displayLink;
-(void)didRotate:(NSNotification*)notification;

@end
