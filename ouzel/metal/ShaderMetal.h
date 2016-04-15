// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "Shader.h"

#ifdef __OBJC__
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
    namespace video
    {
        class RendererMetal;

        class ShaderMetal: public Shader
        {
            friend RendererMetal;
        public:
            virtual ~ShaderMetal();

            virtual bool initFromBuffers(const uint8_t* pixelShader,
                                         uint32_t pixelShaderSize,
                                         const uint8_t* vertexShader,
                                         uint32_t vertexShaderSize,
                                         uint32_t vertexAttributes,
                                         const std::string& pixelShaderFunction = "",
                                         const std::string& vertexShaderFunction = "") override;


            virtual bool setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;
            virtual bool setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0) override;

            void nextBuffers();

            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors) override;
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors) override;
            virtual bool setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices) override;

            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors) override;
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors) override;
            virtual bool setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices) override;

            virtual MTLFunctionPtr getPixelShader() const { return _pixelShader; }
            virtual MTLFunctionPtr getVertexShader() const { return _vertexShader; }

            virtual MTLBufferPtr getPixelShaderConstantBuffer() const { return _pixelShaderConstantBuffer; }
            virtual MTLBufferPtr getVertexShaderConstantBuffer() const { return _vertexShaderConstantBuffer; }
            virtual MTLVertexDescriptorPtr getVertexDescriptor() const { return _vertexDescriptor;  }

            uint32_t getPixelShaderConstantBufferOffset() const { return _pixelShaderConstantBufferOffset; }
            uint32_t getVertexShaderConstantBufferOffset() const { return _vertexShaderConstantBufferOffset; }

        protected:
            ShaderMetal();

            void destroy();
            bool createPixelShaderConstantBuffer(uint32_t size);
            bool createVertexShaderConstantBuffer(uint32_t size);

            bool uploadData(MTLBufferPtr buffer, uint32_t offset, const void* data, uint32_t size);

            MTLFunctionPtr _pixelShader = Nil;
            MTLFunctionPtr _vertexShader = Nil;

            MTLBufferPtr _pixelShaderConstantBuffer = Nil;
            std::vector<char> _pixelShaderData;

            MTLBufferPtr _vertexShaderConstantBuffer = Nil;
            std::vector<char> _vertexShaderData;

            MTLVertexDescriptorPtr _vertexDescriptor = Nil;

            std::vector<uint32_t> _pixelShaderConstantLocations;
            std::vector<uint32_t> _vertexShaderConstantLocations;
            uint32_t _pixelShaderConstantBufferOffset = 0;
            uint32_t _vertexShaderConstantBufferOffset = 0;
        };
    } // namespace video
} // namespace ouzel
