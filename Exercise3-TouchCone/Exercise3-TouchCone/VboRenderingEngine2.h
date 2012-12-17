//
//  VboRenderingEngine2.h
//  Exercise3-TouchCone
//
//  Created by ZhaoyangSu on 12-12-16.
//  Copyright (c) 2012年 su_zhaoyang. All rights reserved.
//

#ifndef __Exercise3_TouchCone__VboRenderingEngine2__
#define __Exercise3_TouchCone__VboRenderingEngine2__

#include <iostream>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <vector>
#include "IRenderingEngine.hpp"

using namespace std;
struct Vertex
{
    vec3 Position;
    vec4 Color;
};

class VboRenderingEngine2 : public IRenderingEngine
{
public:
    VboRenderingEngine2();
    void Initialize(int width, int height);
    void Render() const;
    void UpdateAnimation(float timestep) {}
    void OnRotate(DeviceOrientation newOritation){}
    void OnFingerUp(ivec2 location);
    void OnFingerDown(ivec2 location);
    void OnFingerMove(ivec2 oldLocation, ivec2 newLocation);

private:
    GLuint BuildShader(const char *source,GLenum shaderType)const;
    GLuint BuildProgram(const char *vShader, const char *fShader)const;
    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    GLfloat m_rotationAngle;
    GLfloat m_scale;
    ivec2 m_pivotPoint;
    GLuint m_simpleProgram;
    GLuint m_framebuffer;
    GLuint m_colorRenderbuffer;
    GLuint m_depthRenderbuffer;
    int m_diskIndexCount;
    int m_bodyIndexCount;
};

IRenderingEngine *CreateVboRenderer2()
{
    return new VboRenderingEngine2();
}


#endif /* defined(__Exercise3_TouchCone__VboRenderingEngine2__) */
