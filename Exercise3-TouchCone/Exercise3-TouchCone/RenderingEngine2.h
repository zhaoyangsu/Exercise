//
//  RenderingEngine2.h
//  Exercise3-TouchCone
//
//  Created by ZhaoyangSu on 12-11-13.
//  Copyright (c) 2012年 su_zhaoyang. All rights reserved.
//

#ifndef __Exercise3_TouchCone__RenderingEngine2__
#define __Exercise3_TouchCone__RenderingEngine2__

#include <iostream>
#include <vector>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "IRenderingEngine.hpp"

using namespace std;


struct Vertex {
    vec3 Position;
    vec4 Color;
};


class RenderingEngine2: public IRenderingEngine
{
public:
    RenderingEngine2();
    void Initialize(int width, int height) ;
    void Render() const;
    void UpdateAnimation(float timeStep){};
    void OnRotate (DeviceOrientation newOrientation){} ;
    void OnFingerUp(ivec2 location) ;
    void OnFingerDown(ivec2 location) ;
    void OnFingerMove(ivec2 oldLocation, ivec2 newLocation) ;
private:
    GLuint BuildShader(const char *source, GLenum shaderType) const;
    GLuint BuildProgram(const char *vShader, const char *fShader) const;
    vector<Vertex> m_coneVertices;
    vector<GLubyte> m_coneIndices;
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


IRenderingEngine * CreateRenderer2()
{
    return new RenderingEngine2();
}


#endif /* defined(__Exercise3_TouchCone__RenderingEngine2__) */
