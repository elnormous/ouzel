// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX)
    #include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif defined(OUZEL_PLATFORM_ANDROID)
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif defined(OUZEL_PLATFORM_LINUX)
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#endif

#include "MeshBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererOGL;

        class MeshBufferOGL: public MeshBuffer
        {
            friend RendererOGL;
        public:
            virtual ~MeshBufferOGL();
            virtual void free() override;

            virtual bool init() override;
            virtual bool initFromData(const void* newIndices, uint32_t newIndexSize,
                                      uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                      const void* newVertices, uint32_t newVertexAttributes,
                                      uint32_t newVertexCount, bool newDynamicVertexBuffer) override;

            virtual bool setIndexSize(uint32_t indexSize) override;
            virtual bool setVertexAttributes(uint32_t vertexAttributes) override;

            virtual bool uploadIndices(const void* indices, uint32_t indexCount) override;
            virtual bool uploadVertices(const void* vertices, uint32_t vertexCount) override;

            bool bindVertexBuffer();
            bool updateVertexAttributes();

            GLuint getIndexBufferId() const { return indexBufferId; }
            GLuint getVertexBufferId() const { return vertexBufferId; }
            GLuint getVertexArrayId() const { return vertexArrayId; }

            GLenum getIndexFormat() const { return indexFormat; }

        protected:
            MeshBufferOGL();

            bool updateIndexFormat();

            GLuint indexBufferId = 0;
            GLuint vertexBufferId = 0;
            GLuint vertexArrayId = 0;

            GLenum indexFormat = 0;
        };
    } // namespace graphics
} // namespace ouzel
