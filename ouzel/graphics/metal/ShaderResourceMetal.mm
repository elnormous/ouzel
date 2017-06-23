// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#include <algorithm>
#include "ShaderResourceMetal.h"
#include "RendererMetal.h"
#include "core/Engine.h"
#include "files/FileSystem.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderResourceMetal::ShaderResourceMetal()
        {
        }

        ShaderResourceMetal::~ShaderResourceMetal()
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

        static MTLVertexFormat getVertexFormat(DataType dataType, bool normalized)
        {
            switch (dataType)
            {
                case DataType::BYTE_VECTOR2:
                    return normalized ? MTLVertexFormatChar2Normalized : MTLVertexFormatChar2;
                case DataType::BYTE_VECTOR3:
                    return normalized ? MTLVertexFormatChar3Normalized : MTLVertexFormatChar3;
                case DataType::BYTE_VECTOR4:
                    return normalized ? MTLVertexFormatChar4Normalized : MTLVertexFormatChar4;
                case DataType::UNSIGNED_BYTE_VECTOR2:
                    return normalized ? MTLVertexFormatUChar2Normalized : MTLVertexFormatUChar2;
                case DataType::UNSIGNED_BYTE_VECTOR3:
                    return normalized ? MTLVertexFormatUChar3Normalized : MTLVertexFormatUChar3;
                case DataType::UNSIGNED_BYTE_VECTOR4:
                    return normalized ? MTLVertexFormatUChar4Normalized : MTLVertexFormatUChar4;

                case DataType::SHORT_VECTOR2:
                    return normalized ? MTLVertexFormatShort2Normalized : MTLVertexFormatShort2;
                case DataType::SHORT_VECTOR3:
                    return normalized ? MTLVertexFormatShort3Normalized : MTLVertexFormatShort3;
                case DataType::SHORT_VECTOR4:
                    return normalized ? MTLVertexFormatShort4Normalized : MTLVertexFormatShort4;
                case DataType::UNSIGNED_SHORT_VECTOR2:
                    return normalized ? MTLVertexFormatUShort2Normalized : MTLVertexFormatUShort2;
                case DataType::UNSIGNED_SHORT_VECTOR3:
                    return normalized ? MTLVertexFormatUShort3Normalized : MTLVertexFormatUShort3;
                case DataType::UNSIGNED_SHORT_VECTOR4:
                    return normalized ? MTLVertexFormatUShort4Normalized : MTLVertexFormatUShort4;

                case DataType::INTEGER:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatInt;
                case DataType::INTEGER_VECTOR2:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatInt2;
                case DataType::INTEGER_VECTOR3:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatInt3;
                case DataType::INTEGER_VECTOR4:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatInt4;

                case DataType::UNSIGNED_INTEGER:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatUInt;
                case DataType::UNSIGNED_INTEGER_VECTOR2:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatUInt2;
                case DataType::UNSIGNED_INTEGER_VECTOR3:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatUInt3;
                case DataType::UNSIGNED_INTEGER_VECTOR4:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatUInt4;

                case DataType::FLOAT:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatFloat;
                case DataType::FLOAT_VECTOR2:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatFloat2;
                case DataType::FLOAT_VECTOR3:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatFloat3;
                case DataType::FLOAT_VECTOR4:
                    return normalized ? MTLVertexFormatInvalid : MTLVertexFormatFloat4;

                default:
                    return MTLVertexFormatInvalid;
            }
        }

        bool ShaderResourceMetal::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                if (pixelShader)
                {
                    [pixelShader release];
                    pixelShader = Nil;
                }

                if (vertexShader)
                {
                    [vertexShader release];
                    vertexShader = Nil;
                }

                RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

                uint32_t index = 0;
                NSUInteger offset = 0;

                vertexDescriptor = [MTLVertexDescriptor new];

                for (const VertexAttribute& vertexAttribute : vertexAttributes)
                {
                    MTLVertexFormat vertexFormat = getVertexFormat(vertexAttribute.dataType, vertexAttribute.normalized);

                    if (vertexFormat == MTLVertexFormatInvalid)
                    {
                        Log(Log::Level::ERR) << "Invalid vertex format";
                        return false;
                    }

                    vertexDescriptor.attributes[index].format = vertexFormat;
                    vertexDescriptor.attributes[index].offset = offset;
                    vertexDescriptor.attributes[index].bufferIndex = 0;
                    ++index;
                    offset += getDataTypeSize(vertexAttribute.dataType);
                }

                vertexDescriptor.layouts[0].stride = offset;
                vertexDescriptor.layouts[0].stepRate = 1;
                vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

                NSError* err = Nil;

                dispatch_data_t pixelShaderDispatchData = dispatch_data_create(pixelShaderData.data(), pixelShaderData.size(), NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                id<MTLLibrary> pixelShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:pixelShaderDispatchData error:&err];
                dispatch_release(pixelShaderDispatchData);

                if (!pixelShaderLibrary || err != Nil)
                {
                    if (pixelShaderLibrary) [pixelShaderLibrary release];
                    Log(Log::Level::ERR) << "Failed to load pixel shader, " << (err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error");
                    return false;
                }

                pixelShader = [pixelShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:pixelShaderFunction.c_str()])];

                [pixelShaderLibrary release];

                if (!pixelShader || err != Nil)
                {
                    Log(Log::Level::ERR) << "Failed to get function from shader, " << (err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error");
                    return false;
                }

                if (!pixelShaderConstantInfo.empty())
                {
                    pixelShaderConstantLocations.clear();
                    pixelShaderConstantLocations.reserve(pixelShaderConstantInfo.size());

                    pixelShaderConstantSize = 0;

                    for (const Shader::ConstantInfo& info : pixelShaderConstantInfo)
                    {
                        pixelShaderConstantLocations.push_back({pixelShaderConstantSize, info.size});
                        pixelShaderConstantSize += info.size;
                    }
                }

                dispatch_data_t vertexShaderDispatchData = dispatch_data_create(vertexShaderData.data(), vertexShaderData.size(), NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                id<MTLLibrary> vertexShaderLibrary = [rendererMetal->getDevice() newLibraryWithData:vertexShaderDispatchData error:&err];
                dispatch_release(vertexShaderDispatchData);

                if (!vertexShaderLibrary || err != Nil)
                {
                    if (vertexShaderLibrary) [vertexShaderLibrary release];
                    Log(Log::Level::ERR) << "Failed to load vertex shader, " << (err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error");
                    return false;
                }

                vertexShader = [vertexShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:vertexShaderFunction.c_str()])];

                [vertexShaderLibrary release];

                if (!vertexShader || err != Nil)
                {
                    Log(Log::Level::ERR) << "Failed to get function from shader, " << (err ?[err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error");
                    return false;
                }

                if (!vertexShaderConstantInfo.empty())
                {
                    vertexShaderConstantLocations.clear();
                    vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

                    vertexShaderConstantSize = 0;

                    for (const Shader::ConstantInfo& info : vertexShaderConstantInfo)
                    {
                        vertexShaderConstantLocations.push_back({vertexShaderConstantSize, info.size});
                        vertexShaderConstantSize += info.size;
                    }
                }

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
