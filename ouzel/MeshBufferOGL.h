// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX)
#include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#endif

#include "MeshBuffer.h"

namespace ouzel
{
    class RendererOGL;
    
    class MeshBufferOGL: public MeshBuffer
    {
        friend RendererOGL;
    public:
        virtual ~MeshBufferOGL();
        
        virtual bool initFromData(const std::vector<uint16_t>& indices, const std::vector<VertexPCT>& vertices, bool dynamicIndexBuffer = false, bool dynamicVertexBuffer = false) override;
        
        virtual bool uploadIndices(const std::vector<uint16_t>& indices) override;
        virtual bool uploadVertices(const std::vector<VertexPCT>& vertices) override;
        
        GLuint getIndexBufferId() const { return _indexBufferId; }
        GLuint getVertexArrayId() const { return _vertexArrayId; }
        
        GLsizei getIndexCount() const { return _indexCount; }
        
    protected:
        MeshBufferOGL();
        
        GLuint _vertexArrayId = 0;
        GLuint _indexBufferId = 0;
        GLuint _vertexBufferId = 0;
        
        GLsizei _indexCount;
    };
}
