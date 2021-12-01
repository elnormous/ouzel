// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLPROCEDUREGETTER_HPP
#define OUZEL_GRAPHICS_OGLPROCEDUREGETTER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <algorithm>
#include <stdexcept>

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

#if OUZEL_OPENGL_INTERFACE_EGL
#  include "EGL/egl.h"
#elif OUZEL_OPENGL_INTERFACE_WGL
#  include "GL/wglext.h"
#  include "../../platform/winapi/Library.hpp"
#endif

#include "../../core/Engine.hpp"
#include "../../utils/Utils.hpp"

namespace ouzel::graphics::opengl
{
    class ProcedureGetter final
    {
    public:
        ProcedureGetter(ApiVersion version):
            apiVersion{version}
        {
            const auto glGetErrorProc = getProcAddress<PFNGLGETERRORPROC>("glGetError", ApiVersion{1, 0});

            if (apiVersion >= ApiVersion{3, 0})
            {
                const auto glGetIntegervProc = getProcAddress<PFNGLGETINTEGERVPROC>("glGetIntegerv", ApiVersion{1, 0});
                const auto glGetStringiProc = getProcAddress<PFNGLGETSTRINGIPROC>("glGetStringi", ApiVersion{3, 0});

                GLint extensionCount;
                glGetIntegervProc(GL_NUM_EXTENSIONS, &extensionCount);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    logger.log() << Log::Level::warning << "Failed to get OpenGL extension count, error: " + std::to_string(error);
                else
                    for (GLuint i = 0; i < static_cast<GLuint>(extensionCount); ++i)
                    {
                        const auto extensionPtr = glGetStringiProc(GL_EXTENSIONS, i);

                        if (const auto getStringError = glGetErrorProc(); getStringError != GL_NO_ERROR)
                            logger.log() << Log::Level::warning << "Failed to get OpenGL extension, error: " + std::to_string(getStringError);
                        else if (!extensionPtr)
                            logger.log() << Log::Level::warning << "Failed to get OpenGL extension";
                        else
                            extensions.emplace_back(reinterpret_cast<const char*>(extensionPtr));
                    }
            }
            else
            {
                const auto glGetStringProc = getProcAddress<PFNGLGETSTRINGPROC>("glGetString", ApiVersion{1, 0});
                const auto extensionsPtr = glGetStringProc(GL_EXTENSIONS);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    logger.log() << Log::Level::warning << "Failed to get OpenGL extensions, error: " + std::to_string(error);
                else if (!extensionsPtr)
                    logger.log() << Log::Level::warning << "Failed to get OpenGL extensions";
                else
                    extensions = explodeString(reinterpret_cast<const char*>(extensionsPtr), ' ');
            }

            logger.log() << Log::Level::all << "Supported OpenGL extensions: " << extensions;
        }

        template <typename T>
        T get(const char* name, ApiVersion procApiVersion) const
        {
            return (apiVersion >= procApiVersion) ? getProcAddress<T>(name, procApiVersion) : nullptr;
        }

        template <typename T>
        T get(const char* name, const char* extension) const
        {
            return hasExtension(extension) ? getProcAddress<T>(name) : nullptr;
        }

        template <typename T>
        T get(const char* name,
              ApiVersion procApiVersion,
              const std::map<const char*, const char*>& procExtensions) const
        {
            if (apiVersion >= procApiVersion)
                return getProcAddress<T>(name, procApiVersion);
            else
                for (const auto [procName, extension] : procExtensions)
                    if (auto result = get<T>(procName, extension))
                        return result;

            return nullptr;
        }

        bool hasExtension(const char* ext) const noexcept
        {
            return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
        }

    private:
        template <typename T>
        T getProcAddress(const char* name, ApiVersion procApiVersion) const
        {
#if OUZEL_OPENGL_INTERFACE_EGL
#  if OUZEL_OPENGLES
            if  (procApiVersion >= ApiVersion{3, 0})
                return reinterpret_cast<T>(eglGetProcAddress(name));
            else if (auto p = dlsym(RTLD_DEFAULT, name); p)
                return reinterpret_cast<T>(p);
            else
                throw std::runtime_error(dlerror());
#  else
            (void)procApiVersion;
            return reinterpret_cast<T>(eglGetProcAddress(name));
#  endif
#elif OUZEL_OPENGL_INTERFACE_WGL
            return procApiVersion > ApiVersion{1, 1} ?
                reinterpret_cast<T>(wglGetProcAddress(name)) :
                reinterpret_cast<T>(library.getProcAddress(name));
#else
            (void)procApiVersion;
            if (auto p = dlsym(RTLD_DEFAULT, name); p)
                return reinterpret_cast<T>(p);
            else
                throw std::runtime_error(dlerror());
#endif
        }

        template <typename T>
        T getProcAddress(const char* name) const
        {
#if OUZEL_OPENGL_INTERFACE_EGL
            return reinterpret_cast<T>(eglGetProcAddress(name));
#elif OUZEL_OPENGL_INTERFACE_WGL
            return reinterpret_cast<T>(wglGetProcAddress(name));
#else
            if (auto p = dlsym(RTLD_DEFAULT, name); p)
                return reinterpret_cast<T>(p);
            else
                throw std::runtime_error(dlerror());
#endif
        }

        ApiVersion apiVersion;
        std::vector<std::string> extensions;
#if OUZEL_OPENGL_INTERFACE_WGL
        platform::winapi::Library library{"opengl32.dll"};
#endif
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLPROCEDUREGETTER_HPP
