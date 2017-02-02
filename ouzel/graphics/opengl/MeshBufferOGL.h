// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_WINDOWS
    #define NOMINMAX
#endif

#if OUZEL_SUPPORTS_OPENGL
    #define GL_GLEXT_PROTOTYPES 1
    #include "GL/glcorearb.h"
    #include "GL/glext.h"
#elif OUZEL_SUPPORTS_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include "GLES/gl.h"
    #include "GLES2/gl2.h"
    #include "GLES2/gl2ext.h"
    #include "GLES3/gl3.h"
#endif

#include "graphics/MeshBufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferOGL: public MeshBufferResource
        {
        public:
            MeshBufferOGL();
            virtual ~MeshBufferOGL();

            bool bindBuffers();

            GLenum getIndexType() const { return indexType; }
            GLuint getBytesPerIndex() const { return bytesPerIndex; }
            GLuint getVertexArrayId() const { return vertexArrayId; }

        protected:
            virtual bool upload() override;

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
        };
    } // namespace graphics
} // namespace ouzel
