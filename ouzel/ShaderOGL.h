// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX)
    #include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
#elif defined(OUZEL_PLATFORM_ANDROID)
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif

    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif defined(OUZEL_PLATFORM_LINUX)
    #include <GL/gl.h>
	#include <GL/glx.h>
	#ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
	#include <GL/glext.h>
#endif

#include "Shader.h"

namespace ouzel
{
    class RendererOGL;

    class ShaderOGL: public Shader
    {
        friend RendererOGL;
    public:
        virtual ~ShaderOGL();

        virtual bool initFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes) override;

        GLuint getProgramId() const { return _programId; }

        virtual uint32_t getPixelShaderConstantId(const std::string& name) override;
        virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors) override;
        virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors) override;
        virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices) override;

        virtual uint32_t getVertexShaderConstantId(const std::string& name) override;
        virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors) override;
        virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors) override;
        virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices) override;

    protected:
        ShaderOGL();

        void clean();
        void printShaderMessage(GLuint shaderId);
        void printProgramMessage(GLuint programId);

        GLuint _vertexShaderId = 0;
        GLuint _fragmentShaderId = 0;
        GLuint _programId = 0;
    };
}
