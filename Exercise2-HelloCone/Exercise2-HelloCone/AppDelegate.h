//
//  AppDelegate.h
//  Exercise2-HelloCone
//
//  Created by su_zhaoyang on 12-9-24.
//  Copyright (c) 2012年 su_zhaoyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GLView.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>
{
    GLView* glView;
}

@property (strong, nonatomic) UIWindow *window;

@end
