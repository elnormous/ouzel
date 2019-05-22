// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLBUFFER_HPP
#define OUZEL_GRAPHICS_OGLBUFFER_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "graphics/opengl/OGL.h"

#if OUZEL_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "graphics/opengl/OGLRenderResource.hpp"
#include "graphics/Buffer.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDevice;

        class OGLBuffer final: public OGLRenderResource
        {
        public:
            OGLBuffer(OGLRenderDevice& renderDeviceOGL,
                      Buffer::Usage newUsage, uint32_t newFlags,
                      const std::vector<uint8_t>& newData,
                      uint32_t newSize);
            ~OGLBuffer();

            void reload() final;

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

#endif // OUZEL_GRAPHICS_OGLBUFFER_HPP
