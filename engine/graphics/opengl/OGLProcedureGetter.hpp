// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLPROCEDUREGETTER_HPP
#define OUZEL_GRAPHICS_OGLPROCEDUREGETTER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

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
#endif

#include "../../core/Engine.hpp"

namespace ouzel::graphics::opengl
{
    class ProcedureGetter final
    {
    public:
        ProcedureGetter(ApiVersion version):
            apiVersion(version)
        {
            const auto glGetErrorProc = getProcAddress<PFNGLGETERRORPROC>("glGetError", ApiVersion(1, 0));

            if (apiVersion >= ApiVersion(3, 0))
            {
                const auto glGetIntegervProc = getProcAddress<PFNGLGETINTEGERVPROC>("glGetIntegerv", ApiVersion(1, 0));
                const auto glGetStringiProc = getProcAddress<PFNGLGETSTRINGIPROC>("glGetStringi", ApiVersion(3, 0));

                GLint extensionCount;
                glGetIntegervProc(GL_NUM_EXTENSIONS, &extensionCount);

                GLuint error;
                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    logger.log(Log::Level::warning) << "Failed to get OpenGL extension count, error: " + std::to_string(error);
                else
                    for (GLuint i = 0; i < static_cast<GLuint>(extensionCount); ++i)
                    {
                        const auto extensionPtr = glGetStringiProc(GL_EXTENSIONS, i);

                        if ((error = glGetErrorProc()) != GL_NO_ERROR || !extensionPtr)
                            logger.log(Log::Level::warning) << "Failed to get OpenGL extension, error: " + std::to_string(error);
                        else
                            extensions.emplace_back(reinterpret_cast<const char*>(extensionPtr));
                    }
            }
            else
            {
                const auto glGetStringProc = getProcAddress<PFNGLGETSTRINGPROC>("glGetString", ApiVersion(1, 0));
                const auto extensionsPtr = glGetStringProc(GL_EXTENSIONS);

                GLuint error;
                if ((error = glGetErrorProc()) != GL_NO_ERROR || !extensionsPtr)
                    logger.log(Log::Level::warning) << "Failed to get OpenGL extensions, error: " + std::to_string(error);
                else
                    extensions = explodeString(reinterpret_cast<const char*>(extensionsPtr), ' ');
            }

            logger.log(Log::Level::all) << "Supported OpenGL extensions: " << extensions;
        }

        template <typename T>
        T get(const char* name, ApiVersion procApiVersion) const noexcept
        {
            if (apiVersion >= procApiVersion)
                return getProcAddress<T>(name, procApiVersion);
            else
                return nullptr;
        }

        template <typename T>
        T get(const char* name, const char* extension) const noexcept
        {
            if (hasExtension(extension))
                return getProcAddress<T>(name);
            else
                return nullptr;
        }

        template <typename T>
        T get(const std::map<const char*, const char*>& procExtensions) const noexcept
        {
            for (const auto& extension : procExtensions)
                if (auto result = get<T>(extension.first, extension.second))
                    return result;

            return nullptr;
        }

        template <typename T>
        T get(const char* name,
              ApiVersion procApiVersion,
              const std::map<const char*, const char*>& procExtensions) const noexcept
        {
            if (apiVersion >= procApiVersion)
                return getProcAddress<T>(name, procApiVersion);
            else
                for (const auto& extension : procExtensions)
                    if (auto result = get<T>(extension.first, extension.second))
                        return result;

            return nullptr;
        }

        bool hasExtension(const char* ext) const noexcept
        {
            for (const auto& extension : extensions)
                if (extension == ext) return true;
            return false;
        }

        bool hasExtension(const std::vector<const char*>& ext) const noexcept
        {
            for (const auto& extension : extensions)
                for (auto e : ext)
                    if (extension == e) return true;

            return false;
        }

    private:
        template <typename T>
        T getProcAddress(const char* name, ApiVersion procApiVersion) const noexcept
        {
#if OUZEL_OPENGL_INTERFACE_EGL
#  if OUZEL_OPENGLES
            return procApiVersion >= ApiVersion(3, 0) ?
                reinterpret_cast<T>(eglGetProcAddress(name)) :
                reinterpret_cast<T>(reinterpret_cast<std::uintptr_t>(dlsym(RTLD_DEFAULT, name)));
#  else
            return reinterpret_cast<T>(eglGetProcAddress(name));
#  endif
#elif OUZEL_OPENGL_INTERFACE_WGL
            return procApiVersion > ApiVersion(1, 1) ?
                reinterpret_cast<T>(wglGetProcAddress(name)) :
                reinterpret_cast<T>(GetProcAddress(module.getModule(), name));
#else
            (void)procApiVersion;
            return reinterpret_cast<T>(reinterpret_cast<std::uintptr_t>(dlsym(RTLD_DEFAULT, name)));
#endif
        }

        template <typename T>
        T getProcAddress(const char* name) const noexcept
        {
#if OUZEL_OPENGL_INTERFACE_EGL
            return reinterpret_cast<T>(eglGetProcAddress(name));
#elif OUZEL_OPENGL_INTERFACE_WGL
            return reinterpret_cast<T>(wglGetProcAddress(name));
#else
            return reinterpret_cast<T>(reinterpret_cast<std::uintptr_t>(dlsym(RTLD_DEFAULT, name)));
#endif
        }

        ApiVersion apiVersion;
        std::vector<std::string> extensions;
#if OUZEL_OPENGL_INTERFACE_WGL
        class Module final
        {
        public:
            Module():
                module(LoadLibraryW(L"opengl32.dll"))
            {
                if (!module)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to load opengl32.dll");
            }
            ~Module()
            {
                if (module) FreeLibrary(module);
            }

            Module(const Module&) = delete;
            Module& operator=(const Module&) = delete;

            Module(Module&& other) noexcept:
                module{other.module}
            {
                other.module = nullptr;
            }

            Module& operator=(Module&& other) noexcept
            {
                if (this == &other) return *this;
                if (module) FreeLibrary(module);
                module = other.module;
                other.module = nullptr;
                return *this;
            }

            HMODULE getModule() const noexcept { return module; }
        private:
            HMODULE module = nullptr;
        } module;
#endif
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLPROCEDUREGETTER_HPP
