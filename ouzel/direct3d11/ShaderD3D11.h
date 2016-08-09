// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include <atomic>
#define NOMINMAX
#include <d3d11.h>
#include "graphics/Shader.h"

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

            virtual bool initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                         const std::vector<uint8_t>& newVertexShader,
                                         uint32_t newVertexAttributes,
                                         const std::string& pixelShaderFunction = "",
                                         const std::string& vertexShaderFunction = "") override;

            virtual bool setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;
            virtual bool setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;

            const std::vector<uint32_t>& getPixelShaderConstantLocations() const { return pixelShaderConstantLocations; }
            const std::vector<uint32_t>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            virtual ID3D11PixelShader* getPixelShader() const { return pixelShader; }
            virtual ID3D11VertexShader* getVertexShader() const { return vertexShader; }

            virtual ID3D11Buffer* getPixelShaderConstantBuffer() const { return pixelShaderConstantBuffer; }
            virtual ID3D11Buffer* getVertexShaderConstantBuffer() const { return vertexShaderConstantBuffer; }
            virtual ID3D11InputLayout* getInputLayout() const { return inputLayout;  }

        protected:
            ShaderD3D11();
            virtual bool update() override;

            bool uploadData(ID3D11Buffer* buffer, const void* data, uint32_t size);

            ID3D11PixelShader* pixelShader = nullptr;
            ID3D11VertexShader* vertexShader = nullptr;
            ID3D11InputLayout* inputLayout = nullptr;

            ID3D11Buffer* pixelShaderConstantBuffer = nullptr;
            ID3D11Buffer* vertexShaderConstantBuffer = nullptr;

            std::vector<uint32_t> pixelShaderConstantLocations;
            uint32_t pixelShaderConstantSize = 0;
            std::vector<uint32_t> vertexShaderConstantLocations;
            uint32_t vertexShaderConstantSize = 0;

            std::vector<uint8_t> pixelShaderData;
            std::vector<uint8_t> vertexShaderData;
            std::atomic<bool> dirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
