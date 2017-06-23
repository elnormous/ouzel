// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#include "graphics/ShaderResource.h"

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLFunction> MTLFunctionPtr;
typedef MTLVertexDescriptor* MTLVertexDescriptorPtr;
#else
#include <objc/objc.h>
typedef id MTLFunctionPtr;
typedef id MTLVertexDescriptorPtr;
#endif

namespace ouzel
{
    namespace graphics
    {
        class ShaderResourceMetal: public ShaderResource
        {
        public:
            ShaderResourceMetal();
            virtual ~ShaderResourceMetal();

            struct Location
            {
                uint32_t offset;
                uint32_t size;
            };

            const std::vector<Location>& getPixelShaderConstantLocations() const { return pixelShaderConstantLocations; }
            const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            MTLFunctionPtr getPixelShader() const { return pixelShader; }
            MTLFunctionPtr getVertexShader() const { return vertexShader; }

            MTLVertexDescriptorPtr getVertexDescriptor() const { return vertexDescriptor;  }

            uint32_t getPixelShaderConstantBufferSize() const { return pixelShaderConstantSize; }
            uint32_t getVertexShaderConstantBufferSize() const { return vertexShaderConstantSize; }

        protected:
            virtual bool upload() override;

            bool createPixelShaderConstantBuffer();
            bool createVertexShaderConstantBuffer();

            MTLFunctionPtr pixelShader = Nil;
            MTLFunctionPtr vertexShader = Nil;

            MTLVertexDescriptorPtr vertexDescriptor = Nil;

            std::vector<Location> pixelShaderConstantLocations;
            uint32_t pixelShaderConstantSize = 0;
            std::vector<Location> vertexShaderConstantLocations;
            uint32_t vertexShaderConstantSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
