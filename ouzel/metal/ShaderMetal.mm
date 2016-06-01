// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Engine.h"
#include "RendererMetal.h"
#include "files/FileSystem.h"
#include "ShaderMetal.h"
#include "RendererMetal.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        static const size_t BUFFER_SIZE = 1024 * 1024;

        ShaderMetal::ShaderMetal():
            Shader()
        {
        }

        ShaderMetal::~ShaderMetal()
        {
            if (vertexShader)
            {
                [vertexShader release];
            }

            if (pixelShader)
            {
                [pixelShader release];
            }
            
            if (vertexDescriptor)
            {
                [vertexDescriptor release];
            }
        }

        void ShaderMetal::free()
        {
            Shader::free();

            if (vertexShader)
            {
                [vertexShader release];
                vertexShader = Nil;
            }

            if (pixelShader)
            {
                [pixelShader release];
                pixelShader = Nil;
            }

            if (vertexDescriptor)
            {
                [vertexDescriptor release];
                vertexDescriptor = Nil;
            }
        }

        bool ShaderMetal::initFromBuffers(const uint8_t* newPixelShader,
                                          uint32_t newPixelShaderSize,
                                          const uint8_t* newVertexShader,
                                          uint32_t newVertexShaderSize,
                                          uint32_t newVertexAttributes,
                                          const std::string& pixelShaderFunction,
                                          const std::string& vertexShaderFunction)
        {
            if (!Shader::initFromBuffers(newPixelShader, newPixelShaderSize, newVertexShader, newVertexShaderSize, newVertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                return false;
            }

            free();

            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            uint32_t index = 0;
            NSUInteger offset = 0;
            
            vertexDescriptor = [MTLVertexDescriptor new];

            if (vertexAttributes & VERTEX_POSITION)
            {
                vertexDescriptor.attributes[index].format = MTLVertexFormatFloat3;
                vertexDescriptor.attributes[index].offset = offset;
                vertexDescriptor.attributes[index].bufferIndex = 0;
                ++index;
                offset += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_COLOR)
            {
                vertexDescriptor.attributes[index].format = MTLVertexFormatUChar4Normalized;
                vertexDescriptor.attributes[index].offset = offset;
                vertexDescriptor.attributes[index].bufferIndex = 0;
                ++index;
                offset += 4 * sizeof(uint8_t);
            }

            if (vertexAttributes & VERTEX_NORMAL)
            {
                vertexDescriptor.attributes[index].format = MTLVertexFormatFloat3;
                vertexDescriptor.attributes[index].offset = offset;
                vertexDescriptor.attributes[index].bufferIndex = 0;
                ++index;
                offset += 3 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD0)
            {
                vertexDescriptor.attributes[index].format = MTLVertexFormatFloat2;
                vertexDescriptor.attributes[index].offset = offset;
                vertexDescriptor.attributes[index].bufferIndex = 0;
                ++index;
                offset += 2 * sizeof(float);
            }

            if (vertexAttributes & VERTEX_TEXCOORD1)
            {
                vertexDescriptor.attributes[index].format = MTLVertexFormatFloat2;
                vertexDescriptor.attributes[index].offset = offset;
                vertexDescriptor.attributes[index].bufferIndex = 0;
                ++index;
                offset += 2 * sizeof(float);
            }

            vertexDescriptor.layouts[0].stride = offset;
            vertexDescriptor.layouts[0].stepRate = 1;
            vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

            NSError* err = Nil;

            dispatch_data_t pixelShaderDispatchData = dispatch_data_create(newPixelShader, newPixelShaderSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            id<MTLLibrary> pixelShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:pixelShaderDispatchData error:&err];
            dispatch_release(pixelShaderDispatchData);

            if (err != Nil)
            {
                if (pixelShaderLibrary) [pixelShaderLibrary release];
                log("Failed to load pixel shader, %s", [err.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]);
                return false;
            }

            pixelShader = [pixelShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:pixelShaderFunction.c_str()]];

            [pixelShaderLibrary release];

            if (!pixelShader)
            {
                log("Failed to get function from shader, %s", [err.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]);
                return false;
            }

            dispatch_data_t vertexShaderDispatchData = dispatch_data_create(newVertexShader, newVertexShaderSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            id<MTLLibrary> vertexShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:vertexShaderDispatchData error:&err];
            dispatch_release(vertexShaderDispatchData);

            if (err != Nil)
            {
                if (vertexShaderLibrary) [vertexShaderLibrary release];
                log("Failed to load vertex shader, %s", [err.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]);
                return false;
            }

            vertexShader = [vertexShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:vertexShaderFunction.c_str()]];

            [vertexShaderLibrary release];

            if (!vertexShader)
            {
                log("Failed to get function from shader, %s", [err.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding]);
                return false;
            }

            if (!createPixelShaderConstantBuffer(sizeof(Matrix4)))
            {
                return false;
            }

            if (!createVertexShaderConstantBuffer(sizeof(Matrix4)))
            {
                return false;
            }

            ready = true;

            return true;
        }

        bool ShaderMetal::setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            Shader::setPixelShaderConstantInfo(constantInfo, alignment);

            pixelShaderConstantLocations.reserve(constantInfo.size());

            uint32_t offset = 0;

            for (const ConstantInfo& info : pixelShaderConstantInfo)
            {
                pixelShaderConstantLocations.push_back(offset);
                offset += info.size;
            }

            if (offset > pixelShaderData.size())
            {
                if (pixelShaderConstantBuffer)
                {
                    [pixelShaderConstantBuffer release];
                    pixelShaderConstantBuffer = Nil;
                }

                ready = createPixelShaderConstantBuffer(offset);

                return ready;
            }

            return true;
        }

        bool ShaderMetal::setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            Shader::setVertexShaderConstantInfo(constantInfo, alignment);

            vertexShaderConstantLocations.reserve(constantInfo.size());

            uint32_t offset = 0;

            for (const ConstantInfo& info : vertexShaderConstantInfo)
            {
                vertexShaderConstantLocations.push_back(offset);
                offset += info.size;
            }

            if (offset > vertexShaderData.size())
            {
                if (vertexShaderConstantBuffer)
                {
                    [vertexShaderConstantBuffer release];
                    vertexShaderConstantBuffer = Nil;
                }

                ready = createVertexShaderConstantBuffer(offset);

                return ready;
            }

            return true;
        }

        void ShaderMetal::nextBuffers()
        {
            if (pixelShaderAlignment)
            {
                pixelShaderConstantBufferOffset += pixelShaderData.size();
                pixelShaderConstantBufferOffset = (pixelShaderConstantBufferOffset / pixelShaderAlignment + 1) * pixelShaderAlignment;

                if (BUFFER_SIZE - pixelShaderConstantBufferOffset < pixelShaderAlignment)
                {
                    pixelShaderConstantBufferOffset = 0;
                }
            }

            if (vertexShaderAlignment)
            {
                vertexShaderConstantBufferOffset += vertexShaderData.size();
                vertexShaderConstantBufferOffset = (vertexShaderConstantBufferOffset / vertexShaderAlignment + 1) * vertexShaderAlignment;

                if (BUFFER_SIZE - vertexShaderConstantBufferOffset < vertexShaderAlignment)
                {
                    vertexShaderConstantBufferOffset = 0;
                }
            }
        }

        bool ShaderMetal::setPixelShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value)
        {
            if (index >= pixelShaderConstantLocations.size()) return false;

            uint32_t location = pixelShaderConstantLocations[index];
            memcpy(pixelShaderData.data() + location, value, size * count);

            return uploadData(pixelShaderConstantBuffer,
                              pixelShaderConstantBufferOffset,
                              pixelShaderData.data(),
                              static_cast<uint32_t>(pixelShaderData.size()));
        }

        bool ShaderMetal::setVertexShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value)
        {
            if (index >= vertexShaderConstantLocations.size()) return false;

            uint32_t location = vertexShaderConstantLocations[index];
            memcpy(vertexShaderData.data() + location, value, size * count);

            return uploadData(vertexShaderConstantBuffer,
                              vertexShaderConstantBufferOffset,
                              vertexShaderData.data(),
                              static_cast<uint32_t>(vertexShaderData.size()));
        }

        bool ShaderMetal::createPixelShaderConstantBuffer(uint32_t size)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            pixelShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:BUFFER_SIZE
                                                                                options:MTLResourceCPUCacheModeWriteCombined];

            if (pixelShaderConstantBuffer == Nil)
            {
                log("Failed to create Metal index buffer");
                return false;
            }

            pixelShaderData.resize(size);

            return true;
        }

        bool ShaderMetal::createVertexShaderConstantBuffer(uint32_t size)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            vertexShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:BUFFER_SIZE
                                                                                 options:MTLResourceCPUCacheModeWriteCombined];

            if (vertexShaderConstantBuffer == Nil)
            {
                log("Failed to create Metal constant buffer");
                return false;
            }

            vertexShaderData.resize(size);

            return true;
        }

        bool ShaderMetal::uploadData(MTLBufferPtr buffer, uint32_t offset, const void* data, uint32_t size)
        {
            char* contents = static_cast<char*>([buffer contents]);
            memcpy((contents + offset), data, size);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
