// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
                Shader(RenderDevice& renderDeviceMetal,
                       const std::vector<uint8_t>& fragmentShaderData,
                       const std::vector<uint8_t>& vertexShaderData,
                       const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                       const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                       const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                       uint32_t initFragmentShaderDataAlignment,
                       uint32_t initVertexShaderDataAlignment,
                       const std::string& fragmentShaderFunction,
                       const std::string& vertexShaderFunction);
                ~Shader();

                struct Location final
                {
                    Location(uint32_t initOffset, uint32_t initSize):
                        offset(initOffset), size(initSize)
                    {
                    }

                    uint32_t offset;
                    uint32_t size;
                };

                inline const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const { return vertexAttributes; }

                inline uint32_t getFragmentShaderAlignment() const { return fragmentShaderAlignment; }
                inline uint32_t getVertexShaderAlignment() const { return vertexShaderAlignment; }

                inline const std::vector<Location>& getFragmentShaderConstantLocations() const { return fragmentShaderConstantLocations; }
                inline const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

                inline MTLFunctionPtr getFragmentShader() const { return fragmentShader; }
                inline MTLFunctionPtr getVertexShader() const { return vertexShader; }

                inline MTLVertexDescriptorPtr getVertexDescriptor() const { return vertexDescriptor; }

                inline uint32_t getFragmentShaderConstantBufferSize() const { return fragmentShaderConstantSize; }
                inline uint32_t getVertexShaderConstantBufferSize() const { return vertexShaderConstantSize; }

            private:
                std::set<Vertex::Attribute::Usage> vertexAttributes;

                std::vector<std::pair<std::string, DataType>> fragmentShaderConstantInfo;
                uint32_t fragmentShaderAlignment = 0;
                std::vector<std::pair<std::string, DataType>> vertexShaderConstantInfo;
                uint32_t vertexShaderAlignment = 0;

                MTLFunctionPtr fragmentShader = nil;
                MTLFunctionPtr vertexShader = nil;

                MTLVertexDescriptorPtr vertexDescriptor = nil;

                std::vector<Location> fragmentShaderConstantLocations;
                uint32_t fragmentShaderConstantSize = 0;
                std::vector<Location> vertexShaderConstantLocations;
                uint32_t vertexShaderConstantSize = 0;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALSHADER_HPP
