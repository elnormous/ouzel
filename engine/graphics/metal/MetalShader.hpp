// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALSHADER_HPP
#define OUZEL_GRAPHICS_METALSHADER_HPP

#include <utility>

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <Metal/Metal.h>
typedef id<MTLFunction> MTLFunctionPtr;
typedef MTLVertexDescriptor* MTLVertexDescriptorPtr;
#else
#  include <objc/objc.h>
typedef id MTLFunctionPtr;
typedef id MTLVertexDescriptorPtr;
#endif

#include "graphics/metal/MetalRenderResource.hpp"
#include "graphics/metal/MetalPointer.hpp"
#include "graphics/DataType.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            class RenderDevice;

            class Shader final: public RenderResource
            {
            public:
                Shader(RenderDevice& initRenderDevice,
                       const std::vector<std::uint8_t>& fragmentShaderData,
                       const std::vector<std::uint8_t>& vertexShaderData,
                       const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                       const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                       const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                       const std::string& fragmentShaderFunction,
                       const std::string& vertexShaderFunction);

                struct Location final
                {
                    Location(std::uint32_t initOffset, std::uint32_t initSize):
                        offset(initOffset), size(initSize)
                    {
                    }

                    std::uint32_t offset;
                    std::uint32_t size;
                };

                inline auto& getVertexAttributes() const noexcept { return vertexAttributes; }

                inline auto getFragmentShaderAlignment() const noexcept { return fragmentShaderAlignment; }
                inline auto getVertexShaderAlignment() const noexcept { return vertexShaderAlignment; }

                inline auto& getFragmentShaderConstantLocations() const noexcept { return fragmentShaderConstantLocations; }
                inline auto& getVertexShaderConstantLocations() const noexcept { return vertexShaderConstantLocations; }

                inline auto& getFragmentShader() const noexcept { return fragmentShader; }
                inline auto& getVertexShader() const noexcept { return vertexShader; }

                inline auto& getVertexDescriptor() const noexcept { return vertexDescriptor; }

                inline auto getFragmentShaderConstantBufferSize() const noexcept { return fragmentShaderConstantSize; }
                inline auto getVertexShaderConstantBufferSize() const noexcept { return vertexShaderConstantSize; }

            private:
                std::set<Vertex::Attribute::Usage> vertexAttributes;

                std::vector<std::pair<std::string, DataType>> fragmentShaderConstantInfo;
                std::uint32_t fragmentShaderAlignment = 0;
                std::vector<std::pair<std::string, DataType>> vertexShaderConstantInfo;
                std::uint32_t vertexShaderAlignment = 0;

                Pointer<MTLFunctionPtr> fragmentShader;
                Pointer<MTLFunctionPtr> vertexShader;

                Pointer<MTLVertexDescriptorPtr> vertexDescriptor;

                std::vector<Location> fragmentShaderConstantLocations;
                std::uint32_t fragmentShaderConstantSize = 0;
                std::vector<Location> vertexShaderConstantLocations;
                std::uint32_t vertexShaderConstantSize = 0;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALSHADER_HPP
