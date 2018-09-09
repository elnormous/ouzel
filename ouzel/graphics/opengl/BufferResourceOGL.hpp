// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
#  define GL_GLEXT_PROTOTYPES 1
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  define GL_GLEXT_PROTOTYPES 1
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "graphics/opengl/RenderResourceOGL.hpp"
#include "graphics/Buffer.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class BufferResourceOGL: public RenderResourceOGL
        {
        public:
            explicit BufferResourceOGL(RenderDeviceOGL& renderDeviceOGL);
            virtual ~BufferResourceOGL();

            void init(Buffer::Usage newUsage, uint32_t newFlags,
                      const std::vector<uint8_t>& newData,
                      uint32_t newSize);

            virtual void reload() override;

            void setData(const std::vector<uint8_t>& newData);

            inline uint32_t getFlags() const { return flags; }
            inline Buffer::Usage getUsage() const { return usage; }
            inline GLsizeiptr getSize() const { return size; }

            inline GLuint getBufferId() const { return bufferId; }
            inline GLuint getBufferType() const { return bufferType; }

        private:
            void createBuffer();

            Buffer::Usage usage;
            uint32_t flags = 0;
            std::vector<uint8_t> data;

            GLuint bufferId = 0;
            GLsizeiptr size = 0;

            GLuint bufferType = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
