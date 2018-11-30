// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef DEPTHSTENCILSTATEOGL_HPP
#define DEPTHSTENCILSTATEOGL_HPP

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

#include "graphics/opengl/RenderResourceOGL.hpp"
#include "graphics/DepthStencilState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class DepthStencilStateOGL final: public RenderResourceOGL
        {
        public:
            DepthStencilStateOGL(RenderDeviceOGL& renderDeviceOGL,
                                 bool initDepthTest,
                                 bool initDepthWrite,
                                 DepthStencilState::CompareFunction initCompareFunction);

            void reload() override {}

            bool getDepthTest() const { return depthTest; }
            GLboolean getDepthMask() const { return depthMask; }
            GLenum getCompareFunction() const { return compareFunction; }

        private:
            bool depthTest = false;
            GLboolean depthMask = GL_TRUE;
            GLenum compareFunction;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // DEPTHSTENCILSTATEOGL_HPP
