// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <algorithm>
#include "ShaderResourceMetal.hpp"
#include "RenderDeviceMetal.hpp"
#include "files/FileSystem.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        ShaderResourceMetal::ShaderResourceMetal(RenderDeviceMetal* aRenderDeviceMetal):
            renderDeviceMetal(aRenderDeviceMetal)
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

        bool ShaderResourceMetal::init(const std::vector<uint8_t>& newPixelShader,
                                       const std::vector<uint8_t>& newVertexShader,
                                       const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                                       const std::vector<Shader::ConstantInfo>& newPixelShaderConstantInfo,
                                       const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                       uint32_t newPixelShaderDataAlignment,
                                       uint32_t newVertexShaderDataAlignment,
                                       const std::string& newPixelShaderFunction,
                                       const std::string& newVertexShaderFunction)
        {
            if (!ShaderResource::init(newPixelShader,
                                      newVertexShader,
                                      newVertexAttributes,
                                      newPixelShaderConstantInfo,
                                      newVertexShaderConstantInfo,
                                      newPixelShaderDataAlignment,
                                      newVertexShaderDataAlignment,
                                      newPixelShaderFunction,
                                      newVertexShaderFunction))
            {
                return false;
            }

            uint32_t index = 0;
            NSUInteger offset = 0;

            vertexDescriptor = [MTLVertexDescriptor new];

            for (const Vertex::Attribute& vertexAttribute : Vertex::ATTRIBUTES)
            {
                if (vertexAttributes.find(vertexAttribute.usage) != vertexAttributes.end())
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
                }

                offset += getDataTypeSize(vertexAttribute.dataType);
            }

            vertexDescriptor.layouts[0].stride = offset;
            vertexDescriptor.layouts[0].stepRate = 1;
            vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

            NSError* err;

            dispatch_data_t pixelShaderDispatchData = dispatch_data_create(pixelShaderData.data(), pixelShaderData.size(), nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            id<MTLLibrary> pixelShaderLibrary = [renderDeviceMetal->getDevice() newLibraryWithData:pixelShaderDispatchData error:&err];
            dispatch_release(pixelShaderDispatchData);

            if (!pixelShaderLibrary || err != nil)
            {
                if (pixelShaderLibrary) [pixelShaderLibrary release];
                Log(Log::Level::ERR) << "Failed to load pixel shader, " << (err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error");
                return false;
            }

            if (pixelShader) [pixelShader release];

            pixelShader = [pixelShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:pixelShaderFunction.c_str()])];

            [pixelShaderLibrary release];

            if (!pixelShader || err != nil)
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

            dispatch_data_t vertexShaderDispatchData = dispatch_data_create(vertexShaderData.data(), vertexShaderData.size(), nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            id<MTLLibrary> vertexShaderLibrary = [renderDeviceMetal->getDevice() newLibraryWithData:vertexShaderDispatchData error:&err];
            dispatch_release(vertexShaderDispatchData);

            if (!vertexShaderLibrary || err != nil)
            {
                if (vertexShaderLibrary) [vertexShaderLibrary release];
                Log(Log::Level::ERR) << "Failed to load vertex shader, " << (err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error");
                return false;
            }

            if (vertexShader) [vertexShader release];

            vertexShader = [vertexShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:vertexShaderFunction.c_str()])];

            [vertexShaderLibrary release];

            if (!vertexShader || err != nil)
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

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
