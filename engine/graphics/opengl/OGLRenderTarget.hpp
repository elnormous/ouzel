// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLRENDERTARGET_HPP
#define OUZEL_GRAPHICS_OGLRENDERTARGET_HPP

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

#include <set>
#include "OGLRenderResource.hpp"
#include "../../math/Color.hpp"

namespace ouzel::graphics::opengl
{
    class RenderDevice;
    class Texture;

    class RenderTarget final: public RenderResource
    {
    public:
        RenderTarget(RenderDevice& initRenderDevice,
                     const std::set<Texture*>& initColorTextures,
                     Texture* initDepthTexture);
        ~RenderTarget() override;

        void reload() final;

        auto getFrameBufferId() const noexcept { return frameBufferId; }

    private:
        void createFrameBuffer();
        GLuint frameBufferId = 0;

        std::set<Texture*> colorTextures;
        Texture* depthTexture = nullptr;
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLRENDERTARGET_HPP
