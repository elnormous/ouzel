// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Engine.h"
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
            dirty(false)
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
            std::lock_guard<std::mutex> lock(dataMutex);

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

        bool ShaderMetal::initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                          const std::vector<uint8_t>& newVertexShader,
                                          uint32_t newVertexAttributes,
                                          const std::string& newPixelShaderFunction,
                                          const std::string& newVertexShaderFunction)
        {
            free();

            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Shader::initFromBuffers(newPixelShader, newVertexShader, newVertexAttributes, newPixelShaderFunction, newVertexShaderFunction))
            {
                return false;
            }

            pixelShaderData = newPixelShader;
            pixelShaderFunction = newPixelShaderFunction;
            vertexShaderData = newVertexShader;
            vertexShaderFunction = newVertexShaderFunction;

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool ShaderMetal::setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Shader::setPixelShaderConstantInfo(constantInfo, alignment))
            {
                return false;
            }

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool ShaderMetal::setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Shader::setVertexShaderConstantInfo(constantInfo, alignment))
            {
                return false;
            }

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        void ShaderMetal::nextBuffers()
        {
            if (pixelShaderAlignment)
            {
                pixelShaderConstantBufferOffset += pixelShaderConstantSize;
                pixelShaderConstantBufferOffset = (pixelShaderConstantBufferOffset / pixelShaderAlignment + 1) * pixelShaderAlignment;

                if (BUFFER_SIZE - pixelShaderConstantBufferOffset < pixelShaderAlignment)
                {
                    pixelShaderConstantBufferOffset = 0;
                }
            }

            if (vertexShaderAlignment)
            {
                vertexShaderConstantBufferOffset += vertexShaderConstantSize;
                vertexShaderConstantBufferOffset = (vertexShaderConstantBufferOffset / vertexShaderAlignment + 1) * vertexShaderAlignment;

                if (BUFFER_SIZE - vertexShaderConstantBufferOffset < vertexShaderAlignment)
                {
                    vertexShaderConstantBufferOffset = 0;
                }
            }
        }

        bool ShaderMetal::uploadData(MTLBufferPtr buffer, uint32_t offset, const void* data, uint32_t size)
        {
            char* contents = static_cast<char*>([buffer contents]);
            memcpy((contents + offset), data, size);

            return true;
        }

        bool ShaderMetal::update()
        {
            if (dirty)
            {
                std::lock_guard<std::mutex> lock(dataMutex);

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

                if (!pixelShader)
                {
                    dispatch_data_t pixelShaderDispatchData = dispatch_data_create(pixelShaderData.data(), pixelShaderData.size(), NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
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
                }

                pixelShaderConstantLocations.clear();
                pixelShaderConstantLocations.reserve(pixelShaderConstantInfo.size());

                pixelShaderConstantSize = 0;

                for (const ConstantInfo& info : pixelShaderConstantInfo)
                {
                    pixelShaderConstantLocations.push_back(pixelShaderConstantSize);
                    pixelShaderConstantSize += info.size;
                }

                if (!pixelShaderConstantBuffer)
                {
                    pixelShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:BUFFER_SIZE
                                                                                        options:MTLResourceCPUCacheModeWriteCombined];

                    if (pixelShaderConstantBuffer == Nil)
                    {
                        log("Failed to create Metal index buffer");
                        return false;
                    }
                }

                if (!vertexShader)
                {
                    dispatch_data_t vertexShaderDispatchData = dispatch_data_create(vertexShaderData.data(), vertexShaderData.size(), NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
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
                }

                vertexShaderConstantLocations.clear();
                vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

                vertexShaderConstantSize = 0;

                for (const ConstantInfo& info : vertexShaderConstantInfo)
                {
                    vertexShaderConstantLocations.push_back(vertexShaderConstantSize);
                    vertexShaderConstantSize += info.size;
                }

                if (!vertexShaderConstantBuffer)
                {
                    vertexShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:BUFFER_SIZE
                                                                                         options:MTLResourceCPUCacheModeWriteCombined];

                    if (vertexShaderConstantBuffer == Nil)
                    {
                        log("Failed to create Metal constant buffer");
                        return false;
                    }
                }

                ready = true;
                dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
