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

#include "graphics/BufferInterface.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RendererOGL;
        class MeshBufferInterfaceOGL;

        class BufferInterfaceOGL: public BufferInterface
        {
            friend MeshBufferInterfaceOGL;
        public:
            BufferInterfaceOGL(RendererOGL* aRendererOGL);
            virtual ~BufferInterfaceOGL();

            virtual bool init(Buffer::Usage newUsage, uint32_t newFlags = 0, uint32_t newSize = 0) override;
            virtual bool init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags = 0) override;

            virtual bool setData(const std::vector<uint8_t>& newData) override;
            
            GLuint getBufferId() const { return bufferId; }
            GLuint getBufferType() const { return bufferType; }

        protected:
            bool createBuffer();

            RendererOGL* rendererOGL;

            GLuint bufferId = 0;
            GLsizeiptr bufferSize = 0;

            GLuint bufferType = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
