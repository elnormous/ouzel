// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "graphics/opengl/OGLRenderResource.hpp"
#include "graphics/DepthStencilState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDevice;

        class OGLDepthStencilState final: public OGLRenderResource
        {
        public:
            OGLDepthStencilState(OGLRenderDevice& renderDeviceOGL,
                                 bool initDepthTest,
                                 bool initDepthWrite,
                                 DepthStencilState::CompareFunction initCompareFunction,
                                 bool initStencilEnabled,
                                 uint32_t initStencilReadMask,
                                 uint32_t initStencilWriteMask,
                                 const DepthStencilState::StencilDescriptor& initFrontFaceStencil,
                                 const DepthStencilState::StencilDescriptor& initBackFaceStencil);

            void reload() override {}

            bool getDepthTest() const { return depthTest; }
            GLboolean getDepthMask() const { return depthMask; }
            GLenum getCompareFunction() const { return compareFunction; }

            bool getStencilTest() const { return stencilTest; }
            GLuint getStencilReadMask() const { return stencilReadMask; }
            GLuint getStencilWriteMask() const { return stencilWriteMask; }
            GLenum getFrontFaceFail() const { return frontFaceFail; }
            GLenum getFrontFaceDepthFail() const { return frontFaceDepthFail; }
            GLenum getFrontFacePass() const { return frontFacePass; }
            GLenum getFrontFaceFunction() const { return frontFaceFunction; }
            GLenum getBackFaceFail() const { return backFaceFail; }
            GLenum getBackFaceDepthFail() const { return backFaceDepthFail; }
            GLenum getBackFacePass() const { return backFacePass; }
            GLenum getBackFaceFunction() const { return backFaceFunction; }

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
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLDEPTHSTENCILSTATE_HPP
