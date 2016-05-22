// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include "CompileConfig.h"
#include "Shader.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11;

        class ShaderD3D11: public Shader
        {
            friend RendererD3D11;
        public:
            virtual ~ShaderD3D11();
            virtual void free() override;

            virtual bool initFromBuffers(const uint8_t* newPixelShader,
                                         uint32_t newPixelShaderSize,
                                         const uint8_t* newVertexShader,
                                         uint32_t newVertexShaderSize,
                                         uint32_t newVertexAttributes,
                                         const std::string& pixelShaderFunction = "",
                                         const std::string& vertexShaderFunction = "") override;

            virtual bool setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;
            virtual bool setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;

            virtual bool setPixelShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value) override;
            virtual bool setVertexShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value) override;
            
            virtual ID3D11PixelShader* getPixelShader() const { return pixelShader; }
            virtual ID3D11VertexShader* getVertexShader() const { return vertexShader; }
            
            virtual ID3D11Buffer* getPixelShaderConstantBuffer() const { return pixelShaderConstantBuffer; }
            virtual ID3D11Buffer* getVertexShaderConstantBuffer() const { return vertexShaderConstantBuffer; }
            virtual ID3D11InputLayout* getInputLayout() const { return inputLayout;  }

        protected:
            ShaderD3D11();

            bool createPixelShaderConstantBuffer(uint32_t size);
            bool createVertexShaderConstantBuffer(uint32_t size);

            bool uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size);

            ID3D11PixelShader* pixelShader = nullptr;
            ID3D11VertexShader* vertexShader = nullptr;
            ID3D11InputLayout* inputLayout = nullptr;

            ID3D11Buffer* pixelShaderConstantBuffer = nullptr;
            std::vector<char> pixelShaderData;

            ID3D11Buffer* vertexShaderConstantBuffer = nullptr;
            std::vector<char> vertexShaderData;

            std::vector<uint32_t> pixelShaderConstantLocations;
            std::vector<uint32_t> vertexShaderConstantLocations;
        };
    } // namespace graphics
} // namespace ouzel
