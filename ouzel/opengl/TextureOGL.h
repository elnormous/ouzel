// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_MACOS
    #include <OpenGL/gl3.h>
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif OUZEL_PLATFORM_ANDROID
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif OUZEL_PLATFORM_LINUX
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#elif OUZEL_PLATFORM_RASPBIAN
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
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

            virtual bool init(const Size2& newSize, bool newDynamic, bool newMipmaps = true, bool newRenderTarget = false) override;
            virtual bool initFromBuffer(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps = true) override;

            GLuint getTextureId() const { return textureId; }

        protected:
            TextureOGL();

            virtual bool uploadData(const void* newData, const Size2& newSize) override;
            virtual bool uploadMipmap(uint32_t level, const void* newData) override;
            bool update();

            GLuint textureId = 0;

            std::vector<uint8_t> data;
            bool dirty = true;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
