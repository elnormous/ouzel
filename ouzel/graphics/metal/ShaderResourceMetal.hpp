// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

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

#include "graphics/metal/RenderResourceMetal.hpp"
#include "graphics/Shader.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class ShaderResourceMetal final: public RenderResourceMetal
        {
        public:
            ShaderResourceMetal(RenderDeviceMetal& renderDeviceMetal,
                                const std::vector<uint8_t>& fragmentShaderData,
                                const std::vector<uint8_t>& vertexShaderData,
                                const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                                const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                                const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                uint32_t newFragmentShaderDataAlignment,
                                uint32_t newVertexShaderDataAlignment,
                                const std::string& fragmentShaderFunction,
                                const std::string& vertexShaderFunction);
            ~ShaderResourceMetal();

            struct Location final
            {
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

            std::vector<Shader::ConstantInfo> fragmentShaderConstantInfo;
            uint32_t fragmentShaderAlignment = 0;
            std::vector<Shader::ConstantInfo> vertexShaderConstantInfo;
            uint32_t vertexShaderAlignment = 0;

            MTLFunctionPtr fragmentShader = nil;
            MTLFunctionPtr vertexShader = nil;

            MTLVertexDescriptorPtr vertexDescriptor = nil;

            std::vector<Location> fragmentShaderConstantLocations;
            uint32_t fragmentShaderConstantSize = 0;
            std::vector<Location> vertexShaderConstantLocations;
            uint32_t vertexShaderConstantSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
