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
    
}