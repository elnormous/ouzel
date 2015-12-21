// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "Shader.h"

namespace ouzel
{
    class ShaderD3D11: public Shader
    {
    public:
        ShaderD3D11(const std::string& fragmentShader, const std::string& vertexShader, Renderer* renderer);
        virtual ~ShaderD3D11();
        
        virtual bool loadFromFiles(const std::string& fragmentShader, const std::string& vertexShader) override;
        virtual bool loadFromStrings(const std::string& fragmentShader, const std::string& vertexShader) override;
        
        virtual uint32_t getPixelShaderConstantId(const std::string& name) override;
        virtual uint32_t getVertexShaderConstantId(const std::string& name) override;
        
    protected:
    };
}
