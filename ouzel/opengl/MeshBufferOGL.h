// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX)
    #include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
#elif defined(OUZEL_PLATFORM_ANDROID)
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif

    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif defined(OUZEL_PLATFORM_LINUX)
    #include <GL/gl.h>
    #include <GL/glx.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/glext.h>
#endif

#include "MeshBuffer.h"

namespace ouzel
{
    namespace video
    {
        class RendererOGL;

        class MeshBufferOGL: public MeshBuffer
        {
            friend RendererOGL;
        public:
            virtual ~MeshBufferOGL();

            virtual bool initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer) override;

            virtual bool uploadIndices(const void* indices, uint32_t indexCount) override;
            virtual bool uploadVertices(const void* vertices, uint32_t vertexCount) override;

            virtual bool setVertexAttributes(uint32_t vertexAttributes) override;
            
            GLuint getIndexBufferId() const { return _indexBufferId; }
            GLuint getVertexArrayId() const { return _vertexArrayId; }

            GLenum getIndexFormat() const { return _indexFormat; }

        protected:
            MeshBufferOGL();
            bool updateVertexAttributes();

            void destroy();

            GLuint _vertexArrayId = 0;
            GLuint _indexBufferId = 0;
            GLuint _vertexBufferId = 0;

            GLenum _indexFormat = 0;
        };
    } // namespace video
} // namespace ouzel
