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

            virtual bool initFromBuffers(const uint8_t* pixelShader,
                                         uint32_t pixelShaderSize,
                                         const uint8_t* vertexShader,
                                         uint32_t vertexShaderSize,
                                         uint32_t vertexAttributes,
                                         const std::string& pixelShaderFunction = "",
                                         const std::string& vertexShaderFunction = "") override;

            virtual bool setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;
            virtual bool setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;

            virtual bool setPixelShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value) override;
            virtual bool setVertexShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value) override;

            GLuint getProgramId() const { return programId; }

        protected:
            ShaderOGL();

            void printShaderMessage(GLuint shaderId);
            void printProgramMessage();

            GLuint pixelShaderId = 0;
            GLuint vertexShaderId = 0;
            GLuint programId = 0;

            std::vector<GLint> pixelShaderConstantLocations;
            std::vector<GLint> vertexShaderConstantLocations;
        };
    } // namespace graphics
} // namespace ouzel
