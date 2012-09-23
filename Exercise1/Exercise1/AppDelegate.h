//
//  AppDelegate.h
//  Exercise1
//
//  Created by xiu on 9/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GLView.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>
{
    GLView* glView;
}

@property (strong, nonatomic) UIWindow *window;

@end
