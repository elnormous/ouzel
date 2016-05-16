// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "Shader.h"

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

            virtual bool initFromBuffers(const uint8_t* newPixelShader,
                                         uint32_t newPixelShaderSize,
                                         const uint8_t* newVertexShader,
                                         uint32_t newVertexShaderSize,
                                         uint32_t newVertexAttributes,
                                         const std::string& pixelShaderFunction = "",
                                         const std::string& vertexShaderFunction = "") override;

            virtual bool setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;
            virtual bool setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;

            void nextBuffers();

            virtual bool setPixelShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value) override;
            virtual bool setVertexShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value) override;

            virtual MTLFunctionPtr getPixelShader() const { return pixelShader; }
            virtual MTLFunctionPtr getVertexShader() const { return vertexShader; }

            virtual MTLBufferPtr getPixelShaderConstantBuffer() const { return pixelShaderConstantBuffer; }
            virtual MTLBufferPtr getVertexShaderConstantBuffer() const { return vertexShaderConstantBuffer; }
            virtual MTLVertexDescriptorPtr getVertexDescriptor() const { return vertexDescriptor;  }

            uint32_t getPixelShaderConstantBufferOffset() const { return pixelShaderConstantBufferOffset; }
            uint32_t getVertexShaderConstantBufferOffset() const { return vertexShaderConstantBufferOffset; }

        protected:
            ShaderMetal();

            bool createPixelShaderConstantBuffer(uint32_t size);
            bool createVertexShaderConstantBuffer(uint32_t size);

            bool uploadData(MTLBufferPtr buffer, uint32_t offset, const void* data, uint32_t size);

            MTLFunctionPtr pixelShader = Nil;
            MTLFunctionPtr vertexShader = Nil;

            MTLBufferPtr pixelShaderConstantBuffer = Nil;
            std::vector<char> pixelShaderData;

            MTLBufferPtr vertexShaderConstantBuffer = Nil;
            std::vector<char> vertexShaderData;

            MTLVertexDescriptorPtr vertexDescriptor = Nil;

            std::vector<uint32_t> pixelShaderConstantLocations;
            std::vector<uint32_t> vertexShaderConstantLocations;
            uint32_t pixelShaderConstantBufferOffset = 0;
            uint32_t vertexShaderConstantBufferOffset = 0;
        };
    } // namespace graphics
} // namespace ouzel
