// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
    #define GL_GLEXT_PROTOTYPES 1
    #include "GLES/gl.h"
    #include "GLES2/gl2.h"
    #include "GLES2/gl2ext.h"
    #include "GLES3/gl3.h"
#else
    #define GL_GLEXT_PROTOTYPES 1
    #include "GL/glcorearb.h"
    #include "GL/glext.h"
#endif

#include "graphics/TextureResource.h"

namespace ouzel
{
    namespace graphics
    {
        class TextureResourceOGL: public TextureResource
        {
        public:
            TextureResourceOGL();
            virtual ~TextureResourceOGL();

            GLuint getTextureId() const { return textureId; }

            GLuint getFrameBufferId() const { return frameBufferId; }

            GLbitfield getClearMask() const { return clearMask; }
            const GLfloat* getFrameBufferClearColor() const { return frameBufferClearColor; }

        protected:
            virtual bool upload() override;

            GLuint textureId = 0;

            GLsizei width = 0;
            GLsizei height = 0;

            GLuint frameBufferId = 0;
            GLuint depthBufferId = 0;

            GLbitfield clearMask = 0;
            GLfloat frameBufferClearColor[4];
        };
    } // namespace graphics
} // namespace ouzel

#endif
