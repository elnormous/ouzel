// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX)
    #include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif defined(OUZEL_PLATFORM_ANDROID)
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif defined(OUZEL_PLATFORM_LINUX)
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#endif

#include "Texture.h"

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

            virtual bool init(const Size2& newSize, bool newDynamic, bool newMipmaps = true, bool newRenderTarget = false) override;
            virtual bool initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps = true) override;

            virtual bool uploadMipmap(uint32_t level, const void* data) override;

            GLuint getTextureId() const { return textureId; }

        protected:
            TextureOGL();

            bool uploadData(const void* data, const Size2& newSize) override;

            GLuint textureId = 0;
        };
    } // namespace graphics
} // namespace ouzel
