// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11SHADER_HPP
#define OUZEL_GRAPHICS_D3D11SHADER_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <vector>
#include <utility>

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <d3d11.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "graphics/direct3d11/D3D11RenderResource.hpp"
#include "graphics/direct3d11/D3D11Pointer.hpp"
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
                Shader(RenderDevice& initRenderDevice,
                       const std::vector<uint8_t>& fragmentShaderData,
                       const std::vector<uint8_t>& vertexShaderData,
                       const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                       const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                       const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                       uint32_t,
                       uint32_t,
                       const std::string& fragmentShaderFunction,
                       const std::string& vertexShaderFunction);

                struct Location final
                {
                    Location(uint32_t initOffset, uint32_t initSize):
                        offset(initOffset), size(initSize)
                    {
                    }

                    uint32_t offset;
                    uint32_t size;
                };

                inline auto& getVertexAttributes() const noexcept { return vertexAttributes; }

                auto& getFragmentShaderConstantLocations() const noexcept { return fragmentShaderConstantLocations; }
                auto& getVertexShaderConstantLocations() const noexcept { return vertexShaderConstantLocations; }

                inline auto& getFragmentShader() const noexcept { return fragmentShader; }
                inline auto& getVertexShader() const noexcept { return vertexShader; }

                inline auto& getFragmentShaderConstantBuffer() const noexcept { return fragmentShaderConstantBuffer; }
                inline auto& getVertexShaderConstantBuffer() const noexcept { return vertexShaderConstantBuffer; }
                inline auto& getInputLayout() const noexcept { return inputLayout; }

            private:
                std::set<Vertex::Attribute::Usage> vertexAttributes;

                std::vector<std::pair<std::string, DataType>> fragmentShaderConstantInfo;
                std::vector<std::pair<std::string, DataType>> vertexShaderConstantInfo;

				Pointer<ID3D11PixelShader> fragmentShader;
				Pointer<ID3D11VertexShader> vertexShader;
				Pointer<ID3D11InputLayout> inputLayout;

				Pointer<ID3D11Buffer> fragmentShaderConstantBuffer;
				Pointer<ID3D11Buffer> vertexShaderConstantBuffer;

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
