// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <vector>

#if OUZEL_SUPPORTS_OPENGLES
#define GL_GLEXT_PROTOTYPES 1
#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "GLES3/gl3.h"
#else
#define GL_GLEXT_PROTOTYPES 1
#include "GL/glcorearb.h"
#include "GL/glext.h"
#endif

#include "graphics/MeshBufferResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;
        class BufferResourceOGL;

        class MeshBufferResourceOGL: public MeshBufferResource
        {
        public:
            explicit MeshBufferResourceOGL(RenderDeviceOGL* initRenderDeviceOGL);
            virtual ~MeshBufferResourceOGL();

            virtual bool init(uint32_t newIndexSize, BufferResource* newIndexBuffer,
                              BufferResource* newVertexBuffer) override;

            virtual bool reload() override;

            virtual bool setIndexSize(uint32_t newIndexSize) override;
            virtual bool setIndexBuffer(BufferResource* newIndexBuffer) override;
            virtual bool setVertexBuffer(BufferResource* newVertexBuffer) override;

            bool bindBuffers();

            inline GLenum getIndexType() const { return indexType; }
            inline GLuint getBytesPerIndex() const { return bytesPerIndex; }
            inline GLuint getVertexArrayId() const { return vertexArrayId; }

            inline BufferResourceOGL* getIndexBufferOGL() const { return indexBufferOGL; }
            inline BufferResourceOGL* getVertexBufferOGL() const { return vertexBufferOGL; }

        protected:
            bool createVertexArray();

            RenderDeviceOGL* renderDeviceOGL;

            GLenum indexType = 0;
            GLuint bytesPerIndex = 0;

            struct VertexAttrib
            {
                GLint size;
                GLenum type;
                GLboolean normalized;
                GLsizei stride;
                const GLvoid* pointer;
            };
            std::vector<VertexAttrib> vertexAttribs;

            GLuint vertexArrayId = 0;

            BufferResourceOGL* indexBufferOGL = nullptr;
            BufferResourceOGL* vertexBufferOGL = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif
