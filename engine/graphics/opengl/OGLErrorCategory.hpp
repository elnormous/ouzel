// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLERRORCATEGORY_HPP
#define OUZEL_GRAPHICS_OGLERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <system_error>
#if OUZEL_OPENGLES
#  include "GLES3/gl32.h"
#else
#  include "GL/glcorearb.h"
#endif

namespace ouzel::graphics::opengl
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "OpenGL";
        }

        std::string message(const int condition) const override
        {
            switch (condition)
            {
                case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
                case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
                case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
                case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
                case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
                case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
                case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
                case GL_CONTEXT_LOST: return "GL_CONTEXT_LOST";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLERRORCATEGORY_HPP
