//
//  IRenderingEngine.hpp
//  Exercise1
//
//  Created by xiu on 9/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Exercise1_IRenderingEngine_hpp
#define Exercise1_IRenderingEngine_hpp


enum DeviceOrientation
{
    DeviceOrientationUnknow,
    DeviceOrientationPortrait,
    DeviceOrientationPortraitUpsideDown,
    DeviceOrientationLandscapeLeft,
    DeviceOrientationLandscapeRight,
    DeviceOrientationFaceUp,
    DeviceOrientationFaceDown,
};

struct IRenderingEngine* CreateRender1();

struct IRenderingEngine* CreateRender2();

struct IRenderingEngine
{
    virtual void Initialize(int width, int height) = 0;
    virtual void Render() const = 0;
    virtual void UpdateAnimation(float timeStep) = 0;
    virtual void OnRotate (DeviceOrientation newOrientation) = 0;
    virtual ~IRenderingEngine(){}
};



#endif
