// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <vector>
#include <d3d11.h>
#include "graphics/ShaderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        class ShaderResourceD3D11: public ShaderResource
        {
        public:
            explicit ShaderResourceD3D11(RenderDeviceD3D11& initRenderDeviceD3D11);
            virtual ~ShaderResourceD3D11();

            virtual void init(const std::vector<uint8_t>& newFragmentShader,
                              const std::vector<uint8_t>& newVertexShader,
                              const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                              const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                              const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                              uint32_t newFragmentShaderDataAlignment = 0,
                              uint32_t newVertexShaderDataAlignment = 0,
                              const std::string& newFragmentShaderFunction = "",
                              const std::string& newVertexShaderFunction = "") override;

            struct Location
            {
                uint32_t offset;
                uint32_t size;
            };

            const std::vector<Location>& getFragmentShaderConstantLocations() const { return fragmentShaderConstantLocations; }
            const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            ID3D11PixelShader* getFragmentShader() const { return fragmentShader; }
            ID3D11VertexShader* getVertexShader() const { return vertexShader; }

            ID3D11Buffer* getFragmentShaderConstantBuffer() const { return fragmentShaderConstantBuffer; }
            ID3D11Buffer* getVertexShaderConstantBuffer() const { return vertexShaderConstantBuffer; }
            ID3D11InputLayout* getInputLayout() const { return inputLayout; }

        private:
            RenderDeviceD3D11& renderDeviceD3D11;

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
