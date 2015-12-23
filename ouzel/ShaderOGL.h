// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX)
#include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES

#endif

#include "Shader.h"

namespace ouzel
{
    class ShaderOGL: public Shader
    {
    public:
        ShaderOGL(const std::string& fragmentShader, const std::string& vertexShader, Renderer* renderer);
        virtual ~ShaderOGL();
        
        virtual bool initFromFiles(const std::string& fragmentShader, const std::string& vertexShader) override;
        virtual bool initFromStrings(const std::string& fragmentShader, const std::string& vertexShader) override;
        
        GLuint getProgramId() const { return _programId; }
        
        virtual uint32_t getPixelShaderConstantId(const std::string& name) override;
        virtual bool setPixelShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count) override;
        virtual bool setPixelShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count) override;
        virtual bool setPixelShaderConstant(uint32_t index, const Matrix4* matrixes, uint32_t count) override;
        
        virtual uint32_t getVertexShaderConstantId(const std::string& name) override;
        virtual bool setVertexShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count) override;
        virtual bool setVertexShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count) override;
        virtual bool setVertexShaderConstant(uint32_t index, const Matrix4* matrixes, uint32_t count) override;
        
    protected:
        bool checkShaderError(GLuint shader);
        
        GLuint _vertexShader;
        GLuint _fragmentShader;
        GLuint _programId;
    };
}
