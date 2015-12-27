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

        virtual ID3D11Buffer* getPixelShaderConstantBuffer() const { return _pixelShaderConstantBuffer; }
        virtual ID3D11Buffer* getVertexShaderConstantBuffer() const { return _vertexShaderConstantBuffer; }
        virtual ID3D11InputLayout* getInputLayout() const { return _inputLayout;  }

        virtual uint32_t getPixelShaderConstantId(const std::string& name) override;
        virtual bool setPixelShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count);
        virtual bool setPixelShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count);
        virtual bool setPixelShaderConstant(uint32_t index, const Matrix4* matrices, uint32_t count);

        virtual uint32_t getVertexShaderConstantId(const std::string& name) override;
        virtual bool setVertexShaderConstant(uint32_t index, const Vector3* vectors, uint32_t count);
        virtual bool setVertexShaderConstant(uint32_t index, const Vector4* vectors, uint32_t count);
        virtual bool setVertexShaderConstant(uint32_t index, const Matrix4* matrices, uint32_t count);
        
    protected:
        virtual bool uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size);

        ID3D11PixelShader* _pixelShader = nullptr;
        ID3D11VertexShader* _vertexShader = nullptr;
        ID3D11InputLayout* _inputLayout = nullptr;

        ID3D11Buffer* _pixelShaderConstantBuffer = nullptr;
        ID3D11Buffer* _vertexShaderConstantBuffer = nullptr;
    };
}
