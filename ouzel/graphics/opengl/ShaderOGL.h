// Copyright (C) 2017 Elviss Strazdins
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
#elif OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    #define GL_GLEXT_PROTOTYPES 1
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#include "graphics/ShaderResource.h"

namespace ouzel
{
    namespace graphics
    {
        class ShaderOGL: public ShaderResource
        {
        public:
            ShaderOGL();
            virtual ~ShaderOGL();
            virtual void free() override;

            struct Location
            {
                GLint location;
                DataType dataType;
            };

            const std::vector<Location>& getPixelShaderConstantLocations() const { return pixelShaderConstantLocations; }
            const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            GLuint getProgramId() const { return programId; }

        protected:
            virtual bool upload() override;

            void printShaderMessage(GLuint shaderId);
            void printProgramMessage();

            GLuint pixelShaderId = 0;
            GLuint vertexShaderId = 0;
            GLuint programId = 0;

            std::vector<Location> pixelShaderConstantLocations;
            std::vector<Location> vertexShaderConstantLocations;
        };
    } // namespace graphics
} // namespace ouzel
