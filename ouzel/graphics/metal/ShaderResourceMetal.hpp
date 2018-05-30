// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLFunction> MTLFunctionPtr;
typedef MTLVertexDescriptor* MTLVertexDescriptorPtr;
#else
#include <objc/objc.h>
typedef id MTLFunctionPtr;
typedef id MTLVertexDescriptorPtr;
#endif

#include "graphics/ShaderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class ShaderResourceMetal: public ShaderResource
        {
        public:
            explicit ShaderResourceMetal(RenderDeviceMetal& initRenderDeviceMetal);
            virtual ~ShaderResourceMetal();

            virtual bool init(const std::vector<uint8_t>& newFragmentShader,
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

            inline const std::vector<Location>& getFragmentShaderConstantLocations() const { return fragmentShaderConstantLocations; }
            inline const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            inline MTLFunctionPtr getFragmentShader() const { return fragmentShader; }
            inline MTLFunctionPtr getVertexShader() const { return vertexShader; }

            inline MTLVertexDescriptorPtr getVertexDescriptor() const { return vertexDescriptor;  }

            inline uint32_t getFragmentShaderConstantBufferSize() const { return fragmentShaderConstantSize; }
            inline uint32_t getVertexShaderConstantBufferSize() const { return vertexShaderConstantSize; }

        private:
            RenderDeviceMetal& renderDeviceMetal;

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
