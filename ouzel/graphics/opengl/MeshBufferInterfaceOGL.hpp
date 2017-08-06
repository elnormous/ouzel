// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

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

#include "graphics/MeshBufferInterface.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RendererOGL;
        class BufferInterfaceOGL;

        class MeshBufferInterfaceOGL: public MeshBufferInterface
        {
        public:
            MeshBufferInterfaceOGL(RendererOGL* aRendererOGL);
            virtual ~MeshBufferInterfaceOGL();

            virtual bool init(uint32_t newIndexSize, BufferInterface* newIndexBuffer,
                              const std::vector<VertexAttribute>& newVertexAttributes, BufferInterface* newVertexBuffer) override;

            virtual bool setIndexSize(uint32_t newIndexSize) override;
            virtual bool setIndexBuffer(BufferInterface* newIndexBuffer) override;
            virtual bool setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes) override;
            virtual bool setVertexBuffer(BufferInterface* newVertexBuffer) override;

            bool bindBuffers();

            GLenum getIndexType() const { return indexType; }
            GLuint getBytesPerIndex() const { return bytesPerIndex; }
            GLuint getVertexArrayId() const { return vertexArrayId; }

            BufferInterfaceOGL* getIndexBufferOGL() const { return indexBufferOGL; }
            BufferInterfaceOGL* getVertexBufferOGL() const { return vertexBufferOGL; }

        protected:
            RendererOGL* rendererOGL;

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

            BufferInterfaceOGL* indexBufferOGL = nullptr;
            BufferInterfaceOGL* vertexBufferOGL = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif
