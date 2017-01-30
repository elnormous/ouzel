// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

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

#include "graphics/BufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferOGL;

        class BufferOGL: public BufferResource
        {
            friend MeshBufferOGL;
        public:
            BufferOGL();
            virtual ~BufferOGL();

            GLuint getBufferId() const { return bufferId; }
            GLuint getBufferType() const { return bufferType; }

        protected:
            bool bindBuffer();
            virtual bool upload() override;

            GLuint bufferId = 0;
            GLsizeiptr bufferSize = 0;

            GLuint bufferType = 0;
        };
    } // namespace graphics
} // namespace ouzel
