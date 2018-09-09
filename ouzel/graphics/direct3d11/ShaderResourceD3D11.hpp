// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <vector>
#include <d3d11.h>
#include "graphics/RenderResource.hpp"
#include "graphics/Shader.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        class ShaderResourceD3D11: public RenderResource
        {
        public:
            explicit ShaderResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11);
            virtual ~ShaderResourceD3D11();

            void init(const std::vector<uint8_t>& newFragmentShader,
                      const std::vector<uint8_t>& newVertexShader,
                      const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                      const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                      const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                      uint32_t newFragmentShaderDataAlignment = 0,
                      uint32_t newVertexShaderDataAlignment = 0,
                      const std::string& newFragmentShaderFunction = "",
                      const std::string& newVertexShaderFunction = "");

            struct Location
            {
                uint32_t offset;
                uint32_t size;
            };

            inline const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const { return vertexAttributes; }

            inline uint32_t getFragmentShaderAlignment() const { return fragmentShaderAlignment; }
            inline uint32_t getVertexShaderAlignment() const { return vertexShaderAlignment; }

            const std::vector<Location>& getFragmentShaderConstantLocations() const { return fragmentShaderConstantLocations; }
            const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            ID3D11PixelShader* getFragmentShader() const { return fragmentShader; }
            ID3D11VertexShader* getVertexShader() const { return vertexShader; }

            ID3D11Buffer* getFragmentShaderConstantBuffer() const { return fragmentShaderConstantBuffer; }
            ID3D11Buffer* getVertexShaderConstantBuffer() const { return vertexShaderConstantBuffer; }
            ID3D11InputLayout* getInputLayout() const { return inputLayout; }

        private:
            std::set<Vertex::Attribute::Usage> vertexAttributes;

            std::vector<uint8_t> fragmentShaderData;
            std::vector<uint8_t> vertexShaderData;
            std::string fragmentShaderFunction;
            std::string vertexShaderFunction;

            std::vector<Shader::ConstantInfo> fragmentShaderConstantInfo;
            uint32_t fragmentShaderAlignment = 0;
            std::vector<Shader::ConstantInfo> vertexShaderConstantInfo;
            uint32_t vertexShaderAlignment = 0;

            ID3D11PixelShader* fragmentShader = nullptr;
            ID3D11VertexShader* vertexShader = nullptr;
            ID3D11InputLayout* inputLayout = nullptr;

            ID3D11Buffer* fragmentShaderConstantBuffer = nullptr;
            ID3D11Buffer* vertexShaderConstantBuffer = nullptr;

            std::vector<Location> fragmentShaderConstantLocations;
            uint32_t fragmentShaderConstantSize = 0;
            std::vector<Location> vertexShaderConstantLocations;
            uint32_t vertexShaderConstantSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
