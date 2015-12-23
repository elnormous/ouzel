// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX)
#include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES

#endif

#include "MeshBuffer.h"

namespace ouzel
{
    class MeshBufferOGL: public MeshBuffer
    {
    public:
        MeshBufferOGL(Renderer* renderer);
        virtual ~MeshBufferOGL();
        
        bool initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices);
        
        GLuint getIndexBufferId() const { return _indexBufferId; }
        GLuint getVertexArrayId() const { return _vertexArrayId; }
        
        GLsizei getCount() const { return _count; }
        
    protected:
        GLuint _vertexArrayId = 0;
        GLuint _indexBufferId = 0;
        GLuint _vertexBufferId = 0;
        
        GLsizei _count;
    };
}
