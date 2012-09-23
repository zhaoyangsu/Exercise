//
//  RenderingEngine2.cpp
//  Exercise1
//
//  Created by xiu on 9/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


#include "IRenderingEngine.hpp"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <cmath>
#include <iostream>

#define STR(A)  #A

#include "../Simple.Vert"
#include "../Simple.frag"

static const float RevolutionsPerSecond = 1;

class RenderingEngine2 : public IRenderingEngine
{
public:
    RenderingEngine2();
    void Initialize(int width, int height);
    void Render() const ;
    void UpdateAnimation(float timeStep) ;
    void OnRotate (DeviceOrientation newOrientation);
private:
    float RotationDirection() const;
    GLuint BuildShader(const char * source, GLenum shaderType) const;
    GLuint BuildProgram(const char* vShader, const char* fShader)   const;
    void ApplyOrtho(float maxX, float maxY) const;
    void ApplyRotation(float degree) const;
    
    float m_desireAngle;
    float m_currentAngle;
    GLuint m_simpleProgram;
    GLuint m_framebuffer;
    GLuint m_renderbuffer;
};


struct IRenderingEngine* CreateRender2()
{
    return  new RenderingEngine2();
}

struct Vertex 
{
    float Position[2];
    float Color[4];
};


const Vertex Vertices[]={
    {{-0.5,-0.866},     {1,1,0.5f,1}},
    {{0.5,-0.866},        {1,1,0.5f,1}},
    {{0,1},             {1,1,0.5f,1}},
    {{-0.5,-0.866},        {0.5f,0.5f,0.5f}},
    {{0.5,-0.866},        {0.5f,0.5f,0.5f}},
    {{0,-0.4f},        {0.5f,0.5f,0.5f}},
    
};


RenderingEngine2::RenderingEngine2()
{
    glGenRenderbuffers(1, &m_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
}

void RenderingEngine2::Initialize(int width, int height)
{
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderbuffer);
    glViewport(0, 0, width, height);
    m_simpleProgram = BuildProgram(SimpleVertexShader, SimpleFragmentShader);
    glUseProgram(m_simpleProgram);
    
    ApplyOrtho(2, 3);
    OnRotate(DeviceOrientationPortrait);
    m_currentAngle = m_desireAngle;
}


void RenderingEngine2::Render() const
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ApplyRotation(m_currentAngle);
    GLuint positionSlot = glGetAttribLocation(m_simpleProgram, "Position");
    GLuint colorSlot = glGetAttribLocation(m_simpleProgram, "SourceColor");
    glEnableVertexAttribArray(positionSlot);
    glEnableVertexAttribArray(colorSlot);
    GLsizei stide = sizeof(Vertex);
    const GLvoid* pCoords = &Vertices[0].Position[0];
    const GLvoid* pColor = &Vertices[0].Color[0];
    glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, stide, pCoords);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, stide, pColor);
    GLsizei vertexCount = sizeof(Vertices) / sizeof(Vertex);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glDisableVertexAttribArray(positionSlot);
    glDisableVertexAttribArray(colorSlot);
    
    
}

GLuint RenderingEngine2::BuildProgram(const char *vShader, const char *fShader) const
{
    GLuint vertexShader = BuildShader(vShader, GL_VERTEX_SHADER);
    GLuint fragmentShader = BuildShader(fShader, GL_FRAGMENT_SHADER);
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentShader);
    glLinkProgram(programHandle);
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE)
    {
        GLchar message[256];
        glGetProgramInfoLog(programHandle, sizeof(message), 0, &message[0]);
        std::cout<<message;
        exit(1);
    }
    return programHandle;
}


GLuint RenderingEngine2::BuildShader(const char * source, GLenum shaderType) const
{
    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle, 1, &source, 0);
    glCompileShader(shaderHandle);
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) 
    {
        GLchar message[256];
        glGetShaderInfoLog(shaderHandle, sizeof(message), 0, &message[0]);
        std::cout<<message;
        exit(1);
    }
    return shaderHandle;
}

void RenderingEngine2::ApplyOrtho(float maxX, float maxY) const
{
    float a = 1.0f / maxX;
    float b = 1.0f / maxY;
    float ortho[16] = {
        a,0,0,0,
        0,b,0,0,
        0,0,-1,0,
        0,0,0,1
        
    };
    GLint projectionUniform = glGetUniformLocation(m_simpleProgram, "Projection");
    glUniformMatrix4fv(projectionUniform, 1, 0, &ortho[0]);
}


void RenderingEngine2::ApplyRotation(float degree) const
{
    float radians = degree * 3.14159f / 180.0f;
    float s = std::sin(radians);
    float c = std::cos(radians);
    float zRotation[16]={
        c,s,0,0,
        -s,c,0,0,
        0,0,1,0,
        0,0,0,1
    };
    GLint modelviewUniform = glGetUniformLocation(m_simpleProgram, "Modelview");
    glUniformMatrix4fv(modelviewUniform, 1, 0, &zRotation[0]);
}


void RenderingEngine2::UpdateAnimation(float timeStep)
{
    float direction = RotationDirection();
    if (direction == 0)
    {
        return;
    }
    
    float degree = timeStep * 360 * RevolutionsPerSecond;
    m_currentAngle += degree * direction;
    
    if (m_currentAngle >= 360)
    {
        m_currentAngle -= 360;
    }
    else if(m_currentAngle < 0)
    {
        m_currentAngle += 360;
    }
    
    if (RotationDirection() != direction)
    {
        m_currentAngle = m_desireAngle;
    }
    
}
void RenderingEngine2::OnRotate (DeviceOrientation newOrientation)
{
    float angle = 0;
    switch (newOrientation)
    {
        case DeviceOrientationLandscapeLeft:
            angle = 270;
            break;
        case DeviceOrientationPortraitUpsideDown:
            angle = 180;
            break;
        case DeviceOrientationLandscapeRight:
            angle = 90;
            break;
            
        default:
            break;
    }
    m_desireAngle = angle;
}

float RenderingEngine2::RotationDirection() const
{
    float delta = m_desireAngle - m_currentAngle;
    if (delta == 0)
    {
        return 0;
    }
    bool counterclockwise = ((delta > 0 && delta <= 180) || (delta < -180));
    return counterclockwise ? +1 : -1;
}





