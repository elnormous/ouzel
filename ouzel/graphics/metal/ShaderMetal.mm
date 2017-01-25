// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "core/Engine.h"
#include "RendererMetal.h"
#include "files/FileSystem.h"
#include "ShaderMetal.h"
#include "RendererMetal.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        static const size_t BUFFER_SIZE = 1024 * 1024;

        ShaderMetal::ShaderMetal()
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

        bool ShaderMetal::uploadBuffer(MTLBufferPtr buffer, uint32_t offset, const void* data, uint32_t size)
        {
            std::copy(static_cast<const char*>(data), static_cast<const char*>(data) + size, static_cast<char*>([buffer contents]) + offset);

            return true;
        }

        bool ShaderMetal::upload()
        {
            if (!ShaderResource::upload())
            {
                return false;
            }

            if (data.dirty)
            {
                RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

                pixelShaderAlignment = data.pixelShaderAlignment;
                vertexShaderAlignment = data.vertexShaderAlignment;

                uint32_t index = 0;
                NSUInteger offset = 0;

                vertexDescriptor = [MTLVertexDescriptor new];

                if (data.vertexAttributes & VERTEX_POSITION)
                {
                    vertexDescriptor.attributes[index].format = MTLVertexFormatFloat3;
                    vertexDescriptor.attributes[index].offset = offset;
                    vertexDescriptor.attributes[index].bufferIndex = 0;
                    ++index;
                    offset += 3 * sizeof(float);
                }

                if (data.vertexAttributes & VERTEX_COLOR)
                {
                    vertexDescriptor.attributes[index].format = MTLVertexFormatUChar4Normalized;
                    vertexDescriptor.attributes[index].offset = offset;
                    vertexDescriptor.attributes[index].bufferIndex = 0;
                    ++index;
                    offset += 4 * sizeof(uint8_t);
                }

                if (data.vertexAttributes & VERTEX_NORMAL)
                {
                    vertexDescriptor.attributes[index].format = MTLVertexFormatFloat3;
                    vertexDescriptor.attributes[index].offset = offset;
                    vertexDescriptor.attributes[index].bufferIndex = 0;
                    ++index;
                    offset += 3 * sizeof(float);
                }

                if (data.vertexAttributes & VERTEX_TEXCOORD0)
                {
                    vertexDescriptor.attributes[index].format = MTLVertexFormatFloat2;
                    vertexDescriptor.attributes[index].offset = offset;
                    vertexDescriptor.attributes[index].bufferIndex = 0;
                    ++index;
                    offset += 2 * sizeof(float);
                }

                if (data.vertexAttributes & VERTEX_TEXCOORD1)
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
                    dispatch_data_t pixelShaderDispatchData = dispatch_data_create(data.pixelShaderData.data(), data.pixelShaderData.size(), NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                    id<MTLLibrary> pixelShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:pixelShaderDispatchData error:&err];
                    dispatch_release(pixelShaderDispatchData);

                    if (!pixelShaderLibrary || err != Nil)
                    {
                        if (pixelShaderLibrary) [pixelShaderLibrary release];
                        Log(Log::Level::ERR) << "Failed to load pixel shader, " << (err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error");
                        return false;
                    }

                    pixelShader = [pixelShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:data.pixelShaderFunction.c_str()])];

                    [pixelShaderLibrary release];

                    if (!pixelShader || err != Nil)
                    {
                        Log(Log::Level::ERR) << "Failed to get function from shader, " << (err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error");
                        return false;
                    }
                }

                if (!data.pixelShaderConstantInfo.empty())
                {
                    pixelShaderConstantLocations.clear();
                    pixelShaderConstantLocations.reserve(data.pixelShaderConstantInfo.size());

                    pixelShaderConstantSize = 0;

                    for (const Shader::ConstantInfo& info : data.pixelShaderConstantInfo)
                    {
                        pixelShaderConstantLocations.push_back({pixelShaderConstantSize, info.size});
                        pixelShaderConstantSize += info.size;
                    }
                }

                if (!pixelShaderConstantBuffer)
                {
                    pixelShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:BUFFER_SIZE
                                                                                        options:MTLResourceCPUCacheModeWriteCombined];

                    if (!pixelShaderConstantBuffer)
                    {
                        Log(Log::Level::ERR) << "Failed to create Metal buffer";
                        return false;
                    }
                }

                if (!vertexShader)
                {
                    dispatch_data_t vertexShaderDispatchData = dispatch_data_create(data.vertexShaderData.data(), data.vertexShaderData.size(), NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                    id<MTLLibrary> vertexShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:vertexShaderDispatchData error:&err];
                    dispatch_release(vertexShaderDispatchData);

                    if (!vertexShaderLibrary || err != Nil)
                    {
                        if (vertexShaderLibrary) [vertexShaderLibrary release];
                        Log(Log::Level::ERR) << "Failed to load vertex shader, " << (err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error");
                        return false;
                    }

                    vertexShader = [vertexShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:data.vertexShaderFunction.c_str()])];

                    [vertexShaderLibrary release];

                    if (!vertexShader || err != Nil)
                    {
                        Log(Log::Level::ERR) << "Failed to get function from shader, " << (err ?[err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error");
                        return false;
                    }
                }

                if (!data.vertexShaderConstantInfo.empty())
                {
                    vertexShaderConstantLocations.clear();
                    vertexShaderConstantLocations.reserve(data.vertexShaderConstantInfo.size());

                    vertexShaderConstantSize = 0;

                    for (const Shader::ConstantInfo& info : data.vertexShaderConstantInfo)
                    {
                        vertexShaderConstantLocations.push_back({vertexShaderConstantSize, info.size});
                        vertexShaderConstantSize += info.size;
                    }
                }

                if (!vertexShaderConstantBuffer)
                {
                    vertexShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:BUFFER_SIZE
                                                                                         options:MTLResourceCPUCacheModeWriteCombined];

                    if (!vertexShaderConstantBuffer)
                    {
                        Log(Log::Level::ERR) << "Failed to create Metal buffer";
                        return false;
                    }
                }

                data.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
