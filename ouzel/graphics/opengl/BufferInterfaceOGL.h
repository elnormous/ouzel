// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

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

#include "graphics/BufferInterface.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferInterfaceOGL;

        class BufferInterfaceOGL: public BufferInterface
        {
            friend MeshBufferInterfaceOGL;
        public:
            BufferInterfaceOGL();
            virtual ~BufferInterfaceOGL();

            virtual bool init(Buffer::Usage newUsage, bool newDynamic = true) override;
            virtual bool init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, bool newDynamic) override;

            virtual bool setData(const std::vector<uint8_t>& newData) override;
            
            GLuint getBufferId() const { return bufferId; }
            GLuint getBufferType() const { return bufferType; }

        protected:
            GLuint bufferId = 0;
            GLsizeiptr bufferSize = 0;

            GLuint bufferType = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
