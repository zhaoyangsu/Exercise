//
//  RenderingEngine2.cpp
//  Exercise3-TouchCone
//
//  Created by ZhaoyangSu on 12-11-13.
//  Copyright (c) 2012年 su_zhaoyang. All rights reserved.
//

#include "RenderingEngine2.h"

#define STRINGIFY(A)  #A
#include "Simple.Vert"
#include "Simple.frag"
#include "Matrix.hpp"



RenderingEngine2::RenderingEngine2() : m_rotationAngle(0),m_scale(1)
{
    glGenRenderbuffers(1,&m_colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
}

void RenderingEngine2::Initialize(int width, int height)
{
    m_pivotPoint = ivec2(width / 2, height / 2);
    
    const float coneRadius = 0.5f;
    const float coneHeight = 1.866f;
    const int coneSlices = 40;
    const float dtheta = TwoPi / coneSlices;
    const int vertexCount = coneSlices * 2 + 1;
    const int diskCenterIndex = vertexCount - 1;
    
    m_bodyIndexCount = coneSlices * 3;
    m_diskIndexCount = coneSlices * 3;
    
    m_coneVertices.resize(vertexCount);
    vector<Vertex>::iterator vertex = m_coneVertices.begin();
    
    for (float theta = 0; vertex != m_coneVertices.end() - 1; theta += dtheta)
    {
        float brightness = abs(sin(theta));
        vec4 color(brightness, brightness,brightness,1);
        
        vertex->Position = vec3(0,1,0);
        vertex->Color = color;
        vertex++;
        
        vertex->Position.x = coneRadius * cos(theta);
        vertex->Position.y = 1 - coneHeight;
        vertex->Color = color;
        vertex++;
    }
    
    vertex->Position = vec3(0,1 - coneHeight, 0);
    vertex->Color = vec4(1,1,1,1);
    
    m_coneIndices.resize(m_bodyIndexCount + m_diskIndexCount);
    vector<GLubyte>::iterator index = m_coneIndices.begin();
    
    for (int i = 0; i < coneSlices * 2; i += 2)
    {
        *index++ = i;
        *index++ = (i + 1) % (2 * coneSlices);
        *index++ = (i + 3) % (2 * coneSlices);
    }
    
    //Disk triangles
    for (int i = 1; i < coneSlices * 2 + 1; i += 2)
    {
        *index ++ = diskCenterIndex;
        *index ++ = i;
        *index ++ = (i + 2) % (2 * coneSlices);
    }
    
    //Crate the depth buffer
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    
    
    
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
    
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
    
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    
    m_simpleProgram = BuildProgram(SimpleVertexShader, SimpleFragmentShader);
    glUseProgram(m_simpleProgram);
    
    GLint projectionUniform = glGetUniformLocation(m_simpleProgram, "Projection");
    mat4 projectionMatrix = mat4::Frustum(-1.6f, 1.6, -2.4, 2.4, 5, 10);
    glUniformMatrix4fv(projectionUniform, 1, 0, projectionMatrix.Pointer());
}


void RenderingEngine2::Render() const
{
    GLuint positionSlot = glGetAttribLocation(m_simpleProgram, "Position");
    GLuint colorSlot = glGetAttribLocation(m_simpleProgram, "SourceColor");
    mat4 rotation = mat4::Rotate(m_rotationAngle);
    mat4 scale = mat4::Scale(m_scale);
    mat4 translation = mat4::Translate(0, 0, -7);
    GLint modelviewUniform = glGetUniformLocation(m_simpleProgram, "Modelview");
    mat4 modelviewMatrix = scale * rotation *translation;
    GLsizei stride = sizeof(Vertex);
    const GLvoid *pCoords = &m_coneVertices[0].Position.x;
    const GLvoid *pColors = &m_coneVertices[0].Color.x;
    
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniformMatrix4fv(modelviewUniform, 1, 0, modelviewMatrix.Pointer());
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, stride, pCoords);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, stride, pColors);
    glEnableVertexAttribArray(positionSlot);
    
    const GLvoid *bodyIndices = &m_coneIndices[0];
    const GLvoid *diskIndices = &m_coneIndices[m_bodyIndexCount];
    glEnableVertexAttribArray(colorSlot);
    glDrawElements(GL_TRIANGLES, m_bodyIndexCount, GL_UNSIGNED_BYTE, bodyIndices);
    glDisableVertexAttribArray(colorSlot);
    glVertexAttrib4f(colorSlot, 1, 1, 1, 1);glDrawElements(GL_TRIANGLES, m_diskIndexCount, GL_UNSIGNED_BYTE, diskIndices);
    glDisableVertexAttribArray(positionSlot);
}

void RenderingEngine2::OnFingerUp(ivec2 location)
{
    m_scale = 1.0f;
}

void RenderingEngine2::OnFingerDown(ivec2 location)
{
    m_scale = 1.5f;
    OnFingerMove(location, location);
}

void RenderingEngine2::OnFingerMove(ivec2 oldLocation, ivec2 newLocation)
{
    vec2 direction = vec2(newLocation - m_pivotPoint).Normalized();
    
    //Filp the Y axis because pixel coords increase towards the bottom
    direction.y = -direction.y;
    
    m_rotationAngle = std::acos(direction.y) * 180.0f / M_PI;
    if (direction.x > 0)
    {
        m_rotationAngle = - m_rotationAngle;
    }
}

GLuint RenderingEngine2::BuildShader(const char *source, GLenum shaderType)const
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
        std::cout << message;
        exit(1);
    }
    return shaderHandle;
}

GLuint RenderingEngine2:: BuildProgram(const char *vShader, const char *fShader) const
{
    GLuint vertexShader = BuildShader(vShader, GL_VERTEX_SHADER);
    GLuint fragmentshader = BuildShader(fShader, GL_FRAGMENT_SHADER);
    
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentshader);
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

