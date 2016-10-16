// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

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

        void ShaderMetal::free()
        {
            Shader::free();

            pixelShaderConstantLocations.clear();
            vertexShaderConstantLocations.clear();

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
            char* contents = static_cast<char*>([buffer contents]);
            memcpy((contents + offset), data, size);

            return true;
        }

        bool ShaderMetal::upload()
        {
            if (uploadData.dirty)
            {
                RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

                pixelShaderAlignment = uploadData.pixelShaderAlignment;
                vertexShaderAlignment = uploadData.vertexShaderAlignment;

                uint32_t index = 0;
                NSUInteger offset = 0;

                vertexDescriptor = [MTLVertexDescriptor new];

                if (uploadData.vertexAttributes & VERTEX_POSITION)
                {
                    vertexDescriptor.attributes[index].format = MTLVertexFormatFloat3;
                    vertexDescriptor.attributes[index].offset = offset;
                    vertexDescriptor.attributes[index].bufferIndex = 0;
                    ++index;
                    offset += 3 * sizeof(float);
                }

                if (uploadData.vertexAttributes & VERTEX_COLOR)
                {
                    vertexDescriptor.attributes[index].format = MTLVertexFormatUChar4Normalized;
                    vertexDescriptor.attributes[index].offset = offset;
                    vertexDescriptor.attributes[index].bufferIndex = 0;
                    ++index;
                    offset += 4 * sizeof(uint8_t);
                }

                if (uploadData.vertexAttributes & VERTEX_NORMAL)
                {
                    vertexDescriptor.attributes[index].format = MTLVertexFormatFloat3;
                    vertexDescriptor.attributes[index].offset = offset;
                    vertexDescriptor.attributes[index].bufferIndex = 0;
                    ++index;
                    offset += 3 * sizeof(float);
                }

                if (uploadData.vertexAttributes & VERTEX_TEXCOORD0)
                {
                    vertexDescriptor.attributes[index].format = MTLVertexFormatFloat2;
                    vertexDescriptor.attributes[index].offset = offset;
                    vertexDescriptor.attributes[index].bufferIndex = 0;
                    ++index;
                    offset += 2 * sizeof(float);
                }

                if (uploadData.vertexAttributes & VERTEX_TEXCOORD1)
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
                    dispatch_data_t pixelShaderDispatchData = dispatch_data_create(uploadData.pixelShaderData.data(), uploadData.pixelShaderData.size(), NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                    id<MTLLibrary> pixelShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:pixelShaderDispatchData error:&err];
                    dispatch_release(pixelShaderDispatchData);

                    if (err != Nil)
                    {
                        if (pixelShaderLibrary) [pixelShaderLibrary release];
                        Log(Log::Level::ERR) << "Failed to load pixel shader, " << [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding];
                        return false;
                    }

                    pixelShader = [pixelShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:uploadData.pixelShaderFunction.c_str()])];

                    [pixelShaderLibrary release];

                    if (!pixelShader)
                    {
                        Log(Log::Level::ERR) << "Failed to get function from shader, " << [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding];
                        return false;
                    }
                }

                pixelShaderConstantLocations.clear();
                pixelShaderConstantLocations.reserve(uploadData.pixelShaderConstantInfo.size());

                pixelShaderConstantSize = 0;

                for (const ConstantInfo& info : uploadData.pixelShaderConstantInfo)
                {
                    pixelShaderConstantLocations.push_back({ pixelShaderConstantSize, info.size });
                    pixelShaderConstantSize += info.size;
                }

                if (!pixelShaderConstantBuffer)
                {
                    pixelShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:BUFFER_SIZE
                                                                                        options:MTLResourceCPUCacheModeWriteCombined];

                    if (pixelShaderConstantBuffer == Nil)
                    {
                        Log(Log::Level::ERR) << "Failed to create Metal index buffer";
                        return false;
                    }
                }

                if (!vertexShader)
                {
                    dispatch_data_t vertexShaderDispatchData = dispatch_data_create(uploadData.vertexShaderData.data(), uploadData.vertexShaderData.size(), NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                    id<MTLLibrary> vertexShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:vertexShaderDispatchData error:&err];
                    dispatch_release(vertexShaderDispatchData);

                    if (err != Nil)
                    {
                        if (vertexShaderLibrary) [vertexShaderLibrary release];
                        Log(Log::Level::ERR) << "Failed to load vertex shader, " << [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding];
                        return false;
                    }

                    vertexShader = [vertexShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:uploadData.vertexShaderFunction.c_str()])];

                    [vertexShaderLibrary release];

                    if (!vertexShader)
                    {
                        Log(Log::Level::ERR) << "Failed to get function from shader, " << [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding];
                        return false;
                    }
                }

                vertexShaderConstantLocations.clear();
                vertexShaderConstantLocations.reserve(uploadData.vertexShaderConstantInfo.size());

                vertexShaderConstantSize = 0;

                for (const ConstantInfo& info : uploadData.vertexShaderConstantInfo)
                {
                    vertexShaderConstantLocations.push_back({ vertexShaderConstantSize, info.size });
                    vertexShaderConstantSize += info.size;
                }

                if (!vertexShaderConstantBuffer)
                {
                    vertexShaderConstantBuffer = [rendererMetal->getDevice() newBufferWithLength:BUFFER_SIZE
                                                                                         options:MTLResourceCPUCacheModeWriteCombined];

                    if (vertexShaderConstantBuffer == Nil)
                    {
                        Log(Log::Level::ERR) << "Failed to create Metal constant buffer";
                        return false;
                    }
                }

                uploadData.dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
