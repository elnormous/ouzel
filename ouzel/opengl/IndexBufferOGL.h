// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

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
#elif OUZEL_PLATFORM_RASPBIAN
    #define GL_GLEXT_PROTOTYPES 1
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#include "graphics/IndexBuffer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererOGL;
        class MeshBufferOGL;
        
        class IndexBufferOGL: public IndexBuffer
        {
            friend RendererOGL;
            friend MeshBufferOGL;
        public:
            virtual ~IndexBufferOGL();
            virtual void free() override;

            GLuint getBufferId() const { return bufferId; }
            GLenum getType() const { return type; }
            GLuint getBytesPerIndex() const { return bytesPerIndex; }
            
        protected:
            IndexBufferOGL();
            bool bindBuffer();
            virtual bool upload() override;

            GLuint bufferId = 0;
            GLenum type = 0;
            GLuint bytesPerIndex = 0;
        };
    }
}
