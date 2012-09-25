//
//  RenderingEngine1.cpp
//  Exercise2-HelloCone
//
//  Created by su_zhaoyang on 12-9-24.
//  Copyright (c) 2012年 su_zhaoyang. All rights reserved.
//
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include "IRenderingEngine.hpp"
#include "Quaternion.hpp"
#include <vector>

static const float AnimationDuration = 0.25f;
using namespace std;
struct Vertex
{
    vec3 Position;
    vec4 Color;
};

struct Animation
{
    Quaternion Start;
    Quaternion End;
    Quaternion Current;
    float Elapsed;
    float Duration;
}


class RenderingEngine1: public IRenderingEngine
{
public:
    RenderingEngine1();
    void Initialize(int width, int height);
    void Render() const;
    void UpdateAnimation(float timeStep);
    void OnRotate (DeviceOrientation newOrientation);
private:
    vector<Vertex> m_cone;
    vector<Vertex> m_disk;
    Animation m_animation;
    GLuint m_framebuffer;
    GLuint m_colorRenderbuffer;
    GLuint m_depthRenderbuffer;
};

IRenderingEngine* CreateRender1()
{
    return new RenderingEngine1();
}

RenderingEngine1::RenderingEngine1()
{
    glGenRenderbuffersOES(1, &m_colorRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_colorRenderbuffer);
}

void RenderingEngine1::Initialize(int width, int height)
{
    const float coneRadius = 0.5f;
    const float coneHeight = 1.866f;
    const int coneSlices = 40;
    
    glGenRenderbuffersOES(1, m_depthRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_depthRenderbuffer);
    glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, width, height);
    
    glGenFramebuffersOES(1, &m_framebuffer);
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_framebuffer);
    
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, m_colorRenderbuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, m_depthRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_colorRenderbuffer);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glFrustumf(-1.6f, 1.6, -2.4, 2.4, 5, 10);
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0, 0, -7);
}






