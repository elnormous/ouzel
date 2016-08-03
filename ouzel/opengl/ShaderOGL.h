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

#include "graphics/Shader.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererOGL;

        class ShaderOGL: public Shader
        {
            friend RendererOGL;
        public:
            virtual ~ShaderOGL();
            virtual void free() override;

            virtual bool initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                         const std::vector<uint8_t>& newVertexShader,
                                         uint32_t newVertexAttributes,
                                         const std::string& pixelShaderFunction = "",
                                         const std::string& vertexShaderFunction = "") override;

            virtual bool setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;
            virtual bool setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;

            const std::vector<GLint>& getPixelShaderConstantLocations() const { return pixelShaderConstantLocations; }
            const std::vector<GLint>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            GLuint getProgramId() const { return programId; }

        protected:
            ShaderOGL();
            bool update();

            void printShaderMessage(GLuint shaderId);
            void printProgramMessage();

            GLuint pixelShaderId = 0;
            GLuint vertexShaderId = 0;
            GLuint programId = 0;

            std::vector<GLint> pixelShaderConstantLocations;
            std::vector<GLint> vertexShaderConstantLocations;

            bool dirty = true;
        };
    } // namespace graphics
} // namespace ouzel
