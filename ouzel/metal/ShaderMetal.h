// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <atomic>
#include <mutex>
#include "core/CompileConfig.h"
#include "graphics/Shader.h"

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLFunction> MTLFunctionPtr;
typedef MTLVertexDescriptor* MTLVertexDescriptorPtr;
typedef id<MTLBuffer> MTLBufferPtr;
#else
#include <objc/objc.h>
typedef id MTLFunctionPtr;
typedef id MTLVertexDescriptorPtr;
typedef id MTLBufferPtr;
#endif

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;

        class ShaderMetal: public Shader
        {
            friend RendererMetal;
        public:
            virtual ~ShaderMetal();
            virtual void free() override;

            struct Location
            {
                uint32_t offset;
                uint32_t size;
            };

            const std::vector<Location>& getPixelShaderConstantLocations() const { return pixelShaderConstantLocations; }
            const std::vector<Location>& getVertexShaderConstantLocations() const { return vertexShaderConstantLocations; }

            void nextBuffers();

            virtual MTLFunctionPtr getPixelShader() const { return pixelShader; }
            virtual MTLFunctionPtr getVertexShader() const { return vertexShader; }

            virtual MTLBufferPtr getPixelShaderConstantBuffer() const { return pixelShaderConstantBuffer; }
            virtual MTLBufferPtr getVertexShaderConstantBuffer() const { return vertexShaderConstantBuffer; }
            virtual MTLVertexDescriptorPtr getVertexDescriptor() const { return vertexDescriptor;  }

            uint32_t getPixelShaderConstantBufferOffset() const { return pixelShaderConstantBufferOffset; }
            uint32_t getVertexShaderConstantBufferOffset() const { return vertexShaderConstantBufferOffset; }

        protected:
            ShaderMetal();
            virtual bool upload() override;

            bool createPixelShaderConstantBuffer();
            bool createVertexShaderConstantBuffer();

            bool uploadBuffer(MTLBufferPtr buffer, uint32_t offset, const void* data, uint32_t size);

            MTLFunctionPtr pixelShader = Nil;
            MTLFunctionPtr vertexShader = Nil;

            MTLBufferPtr pixelShaderConstantBuffer = Nil;
            MTLBufferPtr vertexShaderConstantBuffer = Nil;
            MTLVertexDescriptorPtr vertexDescriptor = Nil;

            std::vector<Location> pixelShaderConstantLocations;
            uint32_t pixelShaderConstantSize = 0;
            uint32_t pixelShaderConstantBufferOffset = 0;
            uint32_t pixelShaderAlignment = 0;

            std::vector<Location> vertexShaderConstantLocations;
            uint32_t vertexShaderConstantSize = 0;
            uint32_t vertexShaderConstantBufferOffset = 0;
            uint32_t vertexShaderAlignment = 0;
        };
    } // namespace graphics
} // namespace ouzel
