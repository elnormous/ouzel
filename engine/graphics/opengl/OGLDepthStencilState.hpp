// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP

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
#include "graphics/CompareFunction.hpp"
#include "graphics/StencilOperation.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
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

                inline auto getDepthTest() const noexcept { return depthTest; }
                inline auto getDepthMask() const noexcept { return depthMask; }
                inline auto getCompareFunction() const noexcept { return compareFunction; }

                inline auto getStencilTest() const noexcept { return stencilTest; }
                inline auto getStencilReadMask() const noexcept { return stencilReadMask; }
                inline auto getStencilWriteMask() const noexcept { return stencilWriteMask; }
                inline auto getFrontFaceFail() const noexcept { return frontFaceFail; }
                inline auto getFrontFaceDepthFail() const noexcept { return frontFaceDepthFail; }
                inline auto getFrontFacePass() const noexcept { return frontFacePass; }
                inline auto getFrontFaceFunction() const noexcept { return frontFaceFunction; }
                inline auto getBackFaceFail() const noexcept { return backFaceFail; }
                inline auto getBackFaceDepthFail() const noexcept { return backFaceDepthFail; }
                inline auto getBackFacePass() const noexcept { return backFacePass; }
                inline auto getBackFaceFunction() const noexcept { return backFaceFunction; }

            private:
                bool depthTest = false;
                GLboolean depthMask = GL_TRUE;
                GLenum compareFunction;
                bool stencilTest = false;
                GLuint stencilReadMask = 0xFFFFFFFF;
                GLuint stencilWriteMask = 0xFFFFFFFF;
                GLenum frontFaceFail;
                GLenum frontFaceDepthFail;
                GLenum frontFacePass;
                GLenum frontFaceFunction;
                GLenum backFaceFail;
                GLenum backFaceDepthFail;
                GLenum backFacePass;
                GLenum backFaceFunction;
            };
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP
