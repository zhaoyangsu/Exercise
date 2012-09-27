//
//  GLView.h
//  Exercise3-TouchCone
//
//  Created by Hunk on 12-9-27.
//  Copyright (c) 2012年 su_zhaoyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "IRenderingEngine.hpp"

@interface GLView : UIView
{
@private
    EAGLContext* m_context;
    IRenderingEngine* m_renderingEngine;
    float m_timestamp;
}

-(void)drawView:(CADisplayLink*)displayLink;
-(void)didRotate:(NSNotification*)notifiction;

@end
