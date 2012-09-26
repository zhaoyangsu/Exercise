//
//  RenderingEngine2.cpp
//  Exercise2-HelloCone
//
//  Created by su_zhaoyang on 12-9-26.
//  Copyright (c) 2012年 su_zhaoyang. All rights reserved.
//

#include "IRenderingEngine.hpp"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "IRenderingEngine.hpp"
#include "Quaternion.hpp"
#include <vector>
#include <iostream>

#define STR(A)  #A

#include "../Simple.frag"
#include "../Simple.Vert"

using namespace std;


static const float AnimationDuration = 0.25f;
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
};


class RenderEngine2:public IRenderingEngine
{
public:
    RenderEngine2();
    void Initialize(int Width, int Height);
    void Render() const;
    void UpdateAnimation(float timeStep);
    void OnRotate(DeviceOrientation newOrientation);
private:
    GLuint BuildShader(const char* source, GLenum shaderType) const;
    GLint BuildProgram(const char* vShader, const char* fShader) const;
    vector<Vertex> m_cone;
    vector<Vertex> m_disk;
    Animation m_animation;
    GLuint m_simpleProgram;
    GLuint m_framebuffer;
    GLuint m_colorRenderbuffer;
    GLuint m_depthRenderbuffer;
};


struct IRenderingEngine* CreateRender2()
{
    return new RenderEngine2();
}

RenderEngine2::RenderEngine2()
{
    glGenRenderbuffers(1, &m_colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
}


void RenderEngine2::Initialize(int Width, int Height)
{
    const float coneRadius = 0.5f;
    const float coneHeight = 1.866f;
    const int coneSlices = 40;
    {
        m_cone.resize((coneSlices + 1) * 2);
        
        vector<Vertex>::iterator vertex = m_cone.begin();
        const float dtheta = TwoPi / coneSlices;
        for (float theta = 0; vertex != m_cone.end(); theta += dtheta)
        {
            float brightness = abs(sin(theta));
            vec4 color(brightness,brightness,brightness,1);
            
            vertex->Position = vec3(0,1,0);
            vertex->Color = color;
            vertex++;
            
            vertex->Position.x = coneRadius * cos(theta);
            vertex->Position.y = 1 - coneHeight;
            vertex->Position.z = coneRadius * sin(theta);
            vertex->Color = color;
            vertex++;
        }
    }
    {
        m_disk.resize(coneSlices + 2);
        
        vector<Vertex>::iterator vertex = m_disk.begin();
        vertex->Color = vec4(0.75,0.75,0.75,1);
        vertex->Position.x = 0;
        vertex->Position.y = 1 - coneHeight;
        vertex->Position.z = 0;
        vertex++;
        
        const float dtheta = TwoPi / coneSlices;
        for (float theta = 0; vertex != m_disk.end();theta += dtheta)
        {
            vertex->Color = vec4(0.75,0.75,0.75,1);
            vertex->Position.x = coneRadius * cos(theta);
            vertex->Position.y = 1 - coneHeight;
            vertex->Position.z = coneRadius * sin(theta);
            vertex++;
        }
    }
    
    //Create the Depth buffer
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH_COMPONENT16,
                          Width, Height);
    
    //Create the framebuffer obj; attach the depth and Color buffers.
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER,
                              m_colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER,
                              m_depthRenderbuffer);
    
    //Bind the color buffer for rendering
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
    
   
    //set up some GL state.
    glViewport(0, 0, Width, Height);
    glEnable(GL_DEPTH_TEST);
    
    //Build the GLGL program
    m_simpleProgram = BuildProgram(SimpleVertexShader, SimpleFragmentShader);
    glUseProgram(m_simpleProgram);
    
    //Set the projection matrix.
    GLuint projectionUniform = glGetUniformLocation(m_simpleProgram, "Projection");
    mat4 projectionMatrix = mat4::Frustum(-1.6f, 1.6f, -2.4, -2.4, 5, 10);
    glUniformMatrix4fv(projectionUniform, 1, 0 , projectionMatrix.Pointer());
}


void RenderEngine2::Render() const
{
    GLuint positionSlot = glGetAttribLocation(m_simpleProgram, "Position");
    GLuint colorSlot = glGetAttribLocation(m_simpleProgram, "SourceColor");
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnableVertexAttribArray(positionSlot);
    glEnableVertexAttribArray(colorSlot);
    
    mat4 rotation(m_animation.Current.ToMatrix());
    mat4 translation = mat4::Translate(0, 0, -7);
    
    //Set the model-View matrix.
    GLint modelviewUniform = glGetUniformLocation(m_simpleProgram, "Modelview");
    mat4 modelviewMatrix = translation * rotation;
    glUniformMatrix4fv(modelviewUniform, 1, 0, modelviewMatrix.Pointer());
    
    // Draw the cone
    {
        GLsizei stride = sizeof(Vertex);
        const GLvoid* pCoords = &m_cone[0].Position.x;
        const GLvoid* pColor = &m_cone[0].Color.x;
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, stride, pCoords);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, stride, pColor);
        glDrawArrays(GL_TRIANGLE_FAN, 0, m_disk.size());
    }
    
    //Draw the disk that caps off the base of the cone
    {
        GLsizei stride = sizeof(Vertex);
        const GLvoid* pCoords = &m_disk[0].Position.x;
        const GLvoid* pColors = &m_disk[0].Color.x;
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, stride, pCoords);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, stride, pColors);
        glDrawArrays(GL_TRIANGLE_FAN, 0, m_disk.size());
    }
    
    glDisableVertexAttribArray(positionSlot);
    glDisableVertexAttribArray(colorSlot);
    
    GLenum status =  glGetError();
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"failed to make complete framebuffer object %x", status);
    }

}


void RenderEngine2::UpdateAnimation(float timeStep)
{
    if (m_animation.Current == m_animation.End)
    {
        return;
    }
    
    m_animation.Elapsed += timeStep;
    if (m_animation.Elapsed >= AnimationDuration)
    {
        m_animation.Current = m_animation.End;
    }
    else
    {
        float mu = m_animation.Elapsed / AnimationDuration;
        m_animation.Current = m_animation.Start.Slerp(mu, m_animation.End);
    }
}


void RenderEngine2::OnRotate(DeviceOrientation newOrientation)
{
    vec3 direction;
    
    switch (newOrientation)
    {
        case DeviceOrientationUnknow:
            direction = vec3(0,1,0);
            break;
        case DeviceOrientationPortrait:
            direction = vec3(0,1,0);
            break;
        case DeviceOrientationPortraitUpsideDown:
            direction = vec3(0,-1,0);
            break;
        case DeviceOrientationFaceDown:
            direction = vec3(0,0,-1);
            break;
        case  DeviceOrientationFaceUp:
            direction = vec3(0,0,1);
            break;
        case DeviceOrientationLandscapeLeft:
            direction = vec3(+1,0,0);
            break;
        case DeviceOrientationLandscapeRight:
            direction = vec3(-1,0,0);
            break;
            
        default:
            break;
    }
    
    m_animation.Elapsed = 0;
    m_animation.Start = m_animation.Current = m_animation.End;
    m_animation.End = Quaternion::CreateFromVectors(vec3(0,1,0), direction);
    
}


GLuint RenderEngine2::BuildShader(const char *source, GLenum shaderType) const
{
    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle, 1, &source, 0);
    glCompileShader(shaderHandle);
    
    GLint compliSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compliSuccess);
    
    if (compliSuccess == GL_FALSE)
    {
        GLchar message[256];
        glGetShaderInfoLog(shaderHandle, sizeof(message), 0, &message[0]);
        std::cout << message;
        exit(1);
    }
    
    return shaderHandle;
}


GLint RenderEngine2::BuildProgram(const char *vShader, const char *fShader) const
{
    GLuint vertexShader = BuildShader(vShader, GL_VERTEX_SHADER);
    GLint fragmentShader = BuildShader(fShader, GL_FRAGMENT_SHADER);
    
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
        std::cout << message;
        exit(1);
    }
    
    return programHandle;
}






