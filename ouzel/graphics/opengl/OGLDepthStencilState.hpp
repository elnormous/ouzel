// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP

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
                                  uint32_t initStencilReadMask,
                                  uint32_t initStencilWriteMask,
                                  StencilOperation initFrontFaceStencilFailureOperation,
                                  StencilOperation initFrontFaceStencilDepthFailureOperation,
                                  StencilOperation initFrontFaceStencilPassOperation,
                                  CompareFunction initFrontFaceStencilCompareFunction,
                                  StencilOperation initBackFaceStencilFailureOperation,
                                  StencilOperation initBackFaceStencilDepthFailureOperation,
                                  StencilOperation initBackFaceStencilPassOperation,
                                  CompareFunction initBackFaceStencilCompareFunction);

                void reload() final {}

                inline auto getDepthTest() const { return depthTest; }
                inline auto getDepthMask() const { return depthMask; }
                inline auto getCompareFunction() const { return compareFunction; }

                inline auto getStencilTest() const { return stencilTest; }
                inline auto getStencilReadMask() const { return stencilReadMask; }
                inline auto getStencilWriteMask() const { return stencilWriteMask; }
                inline auto getFrontFaceFail() const { return frontFaceFail; }
                inline auto getFrontFaceDepthFail() const { return frontFaceDepthFail; }
                inline auto getFrontFacePass() const { return frontFacePass; }
                inline auto getFrontFaceFunction() const { return frontFaceFunction; }
                inline auto getBackFaceFail() const { return backFaceFail; }
                inline auto getBackFaceDepthFail() const { return backFaceDepthFail; }
                inline auto getBackFacePass() const { return backFacePass; }
                inline auto getBackFaceFunction() const { return backFaceFunction; }

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
