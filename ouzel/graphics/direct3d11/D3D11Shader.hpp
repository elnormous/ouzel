// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11SHADER_HPP
#define OUZEL_GRAPHICS_D3D11SHADER_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <vector>
#include <utility>
#include <d3d11.h>
#include "graphics/direct3d11/D3D11RenderResource.hpp"
#include "graphics/DataType.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            class RenderDevice;

            class Shader final: public RenderResource
            {
            public:
                Shader(RenderDevice& renderDevice,
                       const std::vector<uint8_t>& fragmentShaderData,
                       const std::vector<uint8_t>& vertexShaderData,
                       const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                       const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                       const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                       uint32_t,
                       uint32_t,
                       const std::string& fragmentShaderFunction,
                       const std::string& vertexShaderFunction);
                ~Shader();

                struct Location final
                {
                    uint32_t offset;
                    uint32_t size;
                };

                inline const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const { return vertexAttributes; }

                const std::vector<Location>& getFragmentShaderConstantLocations() const { return fragmentShaderConstantLocations; }
                const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

                inline ID3D11PixelShader* getFragmentShader() const { return fragmentShader; }
                inline ID3D11VertexShader* getVertexShader() const { return vertexShader; }

                inline ID3D11Buffer* getFragmentShaderConstantBuffer() const { return fragmentShaderConstantBuffer; }
                inline ID3D11Buffer* getVertexShaderConstantBuffer() const { return vertexShaderConstantBuffer; }
                inline ID3D11InputLayout* getInputLayout() const { return inputLayout; }

            private:
                std::set<Vertex::Attribute::Usage> vertexAttributes;

                std::vector<std::pair<std::string, DataType>> fragmentShaderConstantInfo;
                std::vector<std::pair<std::string, DataType>> vertexShaderConstantInfo;

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
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11SHADER_HPP
