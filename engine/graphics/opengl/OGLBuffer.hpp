// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLBUFFER_HPP
#define OUZEL_GRAPHICS_OGLBUFFER_HPP

#include "../../core/Setup.h"

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
#include "../BufferType.hpp"
#include "../Flags.hpp"

namespace ouzel::graphics::opengl
{
    class RenderDevice;

    class Buffer final: public RenderResource
    {
    public:
        Buffer(RenderDevice& initRenderDevice,
               BufferType initType,
               Flags initFlags,
               const std::vector<std::uint8_t>& initData,
               std::uint32_t initSize);
        ~Buffer() override;

        void reload() final;

        void setData(const std::vector<std::uint8_t>& newData);

        auto getFlags() const noexcept { return flags; }
        auto getType() const noexcept { return type; }
        auto getSize() const noexcept { return size; }

        auto getBufferId() const noexcept { return bufferId; }
        auto getBufferType() const noexcept { return bufferType; }

    private:
        void createBuffer();

        BufferType type;
        Flags flags = Flags::none;
        std::vector<std::uint8_t> data;

        GLuint bufferId = 0;
        GLsizeiptr size = 0;

        GLuint bufferType = 0;
    };
}

#endif

#endif // OUZEL_GRAPHICS_OGLBUFFER_HPP
