// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLSTATECACHE_HPP
#define OUZEL_GRAPHICS_OGLSTATECACHE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <array>
#include <unordered_map>

#include "OGL.h"

#if OUZEL_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#  include "GLES3/gl31.h"
#  include "GLES3/gl32.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

namespace ouzel::graphics::opengl
{
    struct StateCache final
    {
        GLenum frontFace = GL_CCW;

        std::unordered_map<GLenum, std::array<GLuint, 16>> textures{
#if !OUZEL_OPENGLES
            {GL_TEXTURE_1D, std::array<GLuint, 16>{}},
#endif
            {GL_TEXTURE_2D, std::array<GLuint, 16>{}},
            {GL_TEXTURE_3D, std::array<GLuint, 16>{}},
            {GL_TEXTURE_CUBE_MAP, std::array<GLuint, 16>{}}
        };
        GLuint programId = 0;
        GLuint frameBufferId = 0;

        std::unordered_map<GLenum, GLuint> bufferId{
            {GL_ELEMENT_ARRAY_BUFFER, 0},
            {GL_ARRAY_BUFFER, 0}
        };

        bool blendEnabled = false;
        GLenum blendModeRGB = 0;
        GLenum blendModeAlpha = 0;
        GLenum blendSourceFactorRGB = 0;
        GLenum blendDestFactorRGB = 0;
        GLenum blendSourceFactorAlpha = 0;
        GLenum blendDestFactorAlpha = 0;
        GLboolean redMask = GL_TRUE;
        GLboolean greenMask = GL_TRUE;
        GLboolean blueMask = GL_TRUE;
        GLboolean alphaMask = GL_TRUE;

#if !OUZEL_OPENGLES
        GLenum polygonFillMode = GL_FILL;
#endif

        bool scissorTestEnabled = false;
        GLint scissorX = 0;
        GLint scissorY = 0;
        GLsizei scissorWidth = 0;
        GLsizei scissorHeight = 0;

        bool depthTestEnabled = false;
        GLboolean depthMask = GL_TRUE;
        GLenum depthFunc = GL_LESS;
        bool stencilTestEnabled = false;
        GLuint stencilMask = 0xFFFFFFFFU;

        GLint viewportX = 0;
        GLint viewportY = 0;
        GLsizei viewportWidth = 0;
        GLsizei viewportHeight = 0;
        bool cullEnabled = false;
        GLenum cullFace = GL_NONE;

        std::array<GLfloat, 4> clearColor{{0.0F, 0.0F, 0.0F, 0.0F}};
        GLfloat clearDepth = 1.0F;
        GLint clearStencil = 0;
    };
}

#endif

#endif //OUZEL_GRAPHICS_OGLSTATECACHE_HPP
