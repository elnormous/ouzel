// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <atomic>
#include <mutex>
#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_MACOS
    #include <OpenGL/gl3.h>
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif OUZEL_PLATFORM_ANDROID
    #include <GLES2/gl2platform.h>
    #define GL_GLEXT_PROTOTYPES 1
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif OUZEL_PLATFORM_LINUX
    #define GL_GLEXT_PROTOTYPES 1
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#elif OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    #define GL_GLEXT_PROTOTYPES 1
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#include "graphics/Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererOGL;
        class RenderTargetOGL;

        class TextureOGL: public Texture
        {
            friend RendererOGL;
            friend RenderTargetOGL;
        public:
            virtual ~TextureOGL();
            virtual void free() override;

            GLuint getTextureId() const { return textureId; }

        protected:
            TextureOGL();

            virtual bool upload() override;

            GLuint textureId = 0;

            GLsizei width = 0;
            GLsizei height = 0;
        };
    } // namespace graphics
} // namespace ouzel
