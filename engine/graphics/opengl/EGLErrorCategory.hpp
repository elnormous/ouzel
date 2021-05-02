// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_EGLERRORCATEGORY_HPP
#define OUZEL_GRAPHICS_EGLERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <system_error>
#include "EGL/egl.h"

namespace ouzel::graphics::egl
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "EGL";
        }

        std::string message(int condition) const override
        {
            switch (condition)
            {
                case EGL_NOT_INITIALIZED: return "EGL_NOT_INITIALIZED";
                case EGL_BAD_ACCESS: return "EGL_BAD_ACCESS";
                case EGL_BAD_ALLOC: return "EGL_BAD_ALLOC";
                case EGL_BAD_ATTRIBUTE: return "EGL_BAD_ATTRIBUTE";
                case EGL_BAD_CONTEXT: return "EGL_BAD_CONTEXT";
                case EGL_BAD_CONFIG: return "EGL_BAD_CONFIG";
                case EGL_BAD_CURRENT_SURFACE: return "EGL_BAD_CURRENT_SURFACE";
                case EGL_BAD_DISPLAY: return "EGL_BAD_DISPLAY";
                case EGL_BAD_SURFACE: return "EGL_BAD_SURFACE";
                case EGL_BAD_MATCH: return "EGL_BAD_MATCH";
                case EGL_BAD_PARAMETER: return "EGL_BAD_PARAMETER";
                case EGL_BAD_NATIVE_PIXMAP: return "EGL_BAD_NATIVE_PIXMAP";
                case EGL_BAD_NATIVE_WINDOW: return "EGL_BAD_NATIVE_WINDOW";
                case EGL_CONTEXT_LOST: return "EGL_CONTEXT_LOST";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}
#endif

#endif // OUZEL_GRAPHICS_EGLERRORCATEGORY_HPP
