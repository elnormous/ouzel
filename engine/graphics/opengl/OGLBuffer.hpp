// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLBUFFER_HPP
#define OUZEL_GRAPHICS_OGLBUFFER_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGL.h"

#if OUZEL_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "OGLRenderResource.hpp"
#include "graphics/BufferType.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            class RenderDevice;

            class Buffer final: public RenderResource
            {
            public:
                Buffer(RenderDevice& initRenderDevice,
                       BufferType initType,
                       std::uint32_t initFlags,
                       const std::vector<std::uint8_t>& initData,
                       std::uint32_t initSize);
                ~Buffer() override;

                void reload() final;

                void setData(const std::vector<std::uint8_t>& newData);

                inline auto getFlags() const noexcept { return flags; }
                inline auto getType() const noexcept { return type; }
                inline auto getSize() const noexcept { return size; }

                inline auto getBufferId() const noexcept { return bufferId; }
                inline auto getBufferType() const noexcept { return bufferType; }

            private:
                void createBuffer();

                BufferType type;
                std::uint32_t flags = 0;
                std::vector<std::uint8_t> data;

                GLuint bufferId = 0;
                GLsizeiptr size = 0;

                GLuint bufferType = 0;
            };
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLBUFFER_HPP
