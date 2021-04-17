// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP

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
#include "../CompareFunction.hpp"
#include "../StencilOperation.hpp"

namespace ouzel::graphics::opengl
{
    class RenderDevice;

    class DepthStencilState final: public RenderResource
    {
    public:
        DepthStencilState(RenderDevice& initRenderDevice,
                          bool initDepthTest,
                          bool initDepthWrite,
                          CompareFunction initCompareFunction,
                          bool initStencilEnabled,
                          std::uint32_t initStencilReadMask,
                          std::uint32_t initStencilWriteMask,
                          StencilOperation initFrontFaceStencilFailureOperation,
                          StencilOperation initFrontFaceStencilDepthFailureOperation,
                          StencilOperation initFrontFaceStencilPassOperation,
                          CompareFunction initFrontFaceStencilCompareFunction,
                          StencilOperation initBackFaceStencilFailureOperation,
                          StencilOperation initBackFaceStencilDepthFailureOperation,
                          StencilOperation initBackFaceStencilPassOperation,
                          CompareFunction initBackFaceStencilCompareFunction);

        void reload() final {}

        auto getDepthTest() const noexcept { return depthTest; }
        auto getDepthMask() const noexcept { return depthMask; }
        auto getCompareFunction() const noexcept { return compareFunction; }

        auto getStencilTest() const noexcept { return stencilTest; }
        auto getStencilReadMask() const noexcept { return stencilReadMask; }
        auto getStencilWriteMask() const noexcept { return stencilWriteMask; }
        auto getFrontFaceFail() const noexcept { return frontFaceFail; }
        auto getFrontFaceDepthFail() const noexcept { return frontFaceDepthFail; }
        auto getFrontFacePass() const noexcept { return frontFacePass; }
        auto getFrontFaceFunction() const noexcept { return frontFaceFunction; }
        auto getBackFaceFail() const noexcept { return backFaceFail; }
        auto getBackFaceDepthFail() const noexcept { return backFaceDepthFail; }
        auto getBackFacePass() const noexcept { return backFacePass; }
        auto getBackFaceFunction() const noexcept { return backFaceFunction; }

    private:
        bool depthTest = false;
        GLboolean depthMask = GL_TRUE;
        GLenum compareFunction;
        bool stencilTest = false;
        GLuint stencilReadMask = 0xFFFFFFFFU;
        GLuint stencilWriteMask = 0xFFFFFFFFU;
        GLenum frontFaceFail;
        GLenum frontFaceDepthFail;
        GLenum frontFacePass;
        GLenum frontFaceFunction;
        GLenum backFaceFail;
        GLenum backFaceDepthFail;
        GLenum backFacePass;
        GLenum backFaceFunction;
    };
}

#endif

#endif // OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP
