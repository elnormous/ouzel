// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
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

#include "graphics/MeshBufferResource.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferOGL: public MeshBufferResource
        {
        public:
            MeshBufferOGL();
            virtual ~MeshBufferOGL();
            virtual void free() override;

            bool bindBuffers();

            GLenum getIndexType() const { return indexType; }
            GLuint getBytesPerIndex() const { return bytesPerIndex; }
            GLuint getVertexArrayId() const { return vertexArrayId; }

        protected:
            virtual bool upload() override;

            GLenum indexType = 0;
            GLuint bytesPerIndex = 0;

            struct VertexAttrib
            {
                GLint size;
                GLenum type;
                GLboolean normalized;
                GLsizei stride;
                const GLvoid* pointer;
            };
            std::vector<VertexAttrib> vertexAttribs;
            
            GLuint vertexArrayId = 0;
        };
    } // namespace graphics
} // namespace ouzel
