// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <d3d11.h>
#include "CompileConfig.h"
#include "Shader.h"

namespace ouzel
{
    class ShaderD3D11: public Shader
    {
    public:
        ShaderD3D11(Renderer* renderer);
        virtual ~ShaderD3D11();
        
        virtual bool initFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize) override;
        
        virtual ID3D11PixelShader* getPixelShader() const { return _pixelShader; }
        virtual ID3D11VertexShader* getVertexShader() const { return _vertexShader; }

        virtual uint32_t getPixelShaderConstantId(const std::string& name) override;
        virtual uint32_t getVertexShaderConstantId(const std::string& name) override;
        
    protected:
        ID3D11PixelShader* _pixelShader = nullptr;
        ID3D11VertexShader* _vertexShader = nullptr;
        ID3D11InputLayout* _inputLayout = nullptr;
    };
}
