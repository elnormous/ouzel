// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <atomic>
#include <mutex>
#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_MACOS
    #include <OpenGL/gl3.h>
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif OUZEL_PLATFORM_ANDROID
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif OUZEL_PLATFORM_LINUX
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#elif OUZEL_PLATFORM_RASPBIAN
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#include "graphics/MeshBuffer.h"

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

            virtual bool init(bool newDynamicIndexBuffer = true, bool newDynamicVertexBuffer = true) override;
            virtual bool initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                        uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                        const void* newVertices, uint32_t newVertexAttributes,
                                        uint32_t newVertexCount, bool newDynamicVertexBuffer) override;

            virtual bool setIndexSize(uint32_t indexSize) override;
            virtual bool setVertexAttributes(uint32_t vertexAttributes) override;

            virtual bool uploadIndices(const void* newIndices, uint32_t newIndexCount) override;
            virtual bool uploadVertices(const void* newVertices, uint32_t newVertexCount) override;

            bool bindVertexBuffer();

            GLuint getIndexBufferId() const { return indexBufferId; }
            GLuint getVertexBufferId() const { return vertexBufferId; }
            GLuint getVertexArrayId() const { return vertexArrayId; }

            static GLenum getFormat(uint32_t size);
            GLenum getIndexFormat() const { return indexFormat; }

        protected:
            MeshBufferOGL();
            virtual bool update() override;

            GLuint indexBufferId = 0;
            GLuint vertexBufferId = 0;
            GLuint vertexArrayId = 0;

            GLenum indexFormat = 0;

            struct VertexAttrib
            {
                GLint size;
                GLenum type;
                GLboolean normalized;
                GLsizei stride;
                const GLvoid *pointer;
            };
            std::vector<VertexAttrib> vertexAttribs;

            std::vector<uint8_t> indexData;
            std::atomic<bool> indexBufferDirty;
            std::vector<uint8_t> vertexData;
            std::atomic<bool> vertexBufferDirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
