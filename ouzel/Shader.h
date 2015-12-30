// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

namespace ouzel
{
    class Renderer;
    
    class Shader: public Noncopyable, public ReferenceCounted
    {
        friend Renderer;
    public:
        virtual ~Shader();
        
        virtual bool initFromFiles(const std::string& fragmentShader, const std::string& vertexShader);
        virtual bool initFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize);
        
        virtual uint32_t getPixelShaderConstantId(const std::string& name);
        virtual bool setPixelShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count);
        virtual bool setPixelShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count);
        virtual bool setPixelShaderConstant(uint32_t index, const Matrix4* matrices, uint32_t count);
        
        virtual uint32_t getVertexShaderConstantId(const std::string& name);
        virtual bool setVertexShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count);
        virtual bool setVertexShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count);
        virtual bool setVertexShaderConstant(uint32_t index, const Matrix4* matrices, uint32_t count);
        
    protected:
        Shader();
        
        std::string _fragmentShaderFilename;
        std::string _vertexShaderFilename;
    };
}
