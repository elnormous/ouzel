// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <TargetConditionals.h>
#include <algorithm>
#include "MetalShader.hpp"
#include "MetalError.hpp"
#include "MetalRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            namespace
            {
                constexpr MTLVertexFormat getVertexFormat(DataType dataType)
                {
                    switch (dataType)
                    {
                        case DataType::Byte: return MTLVertexFormatInvalid;
                        case DataType::ByteNorm: return MTLVertexFormatInvalid;
                        case DataType::UnsignedByte: return MTLVertexFormatInvalid;
                        case DataType::UnsignedByteNorm: return MTLVertexFormatInvalid;

                        case DataType::ByteVector2: return MTLVertexFormatChar2;
                        case DataType::ByteVector2Norm: return MTLVertexFormatChar2Normalized;
                        case DataType::UnsignedByteVector2: return MTLVertexFormatUChar2;
                        case DataType::UnsignedByteVector2Norm: return MTLVertexFormatUChar2Normalized;

                        case DataType::ByteVector3: return MTLVertexFormatChar3;
                        case DataType::ByteVector3Norm: return MTLVertexFormatChar3Normalized;
                        case DataType::UnsignedByteVector3: return MTLVertexFormatUChar3;
                        case DataType::UnsignedByteVector3Norm: return MTLVertexFormatUChar3Normalized;

                        case DataType::ByteVector4: return MTLVertexFormatChar4;
                        case DataType::ByteVector4Norm: return MTLVertexFormatChar4Normalized;
                        case DataType::UnsignedByteVector4: return MTLVertexFormatUChar4;
                        case DataType::UnsignedByteVector4Norm: return MTLVertexFormatUChar4Normalized;

                        case DataType::Short: return MTLVertexFormatInvalid;
                        case DataType::ShortNorm: return MTLVertexFormatInvalid;
                        case DataType::UnsignedShort: return MTLVertexFormatInvalid;
                        case DataType::UnsignedShortNorm: return MTLVertexFormatInvalid;

                        case DataType::ShortVector2: return MTLVertexFormatShort2;
                        case DataType::ShortVector2Norm: return MTLVertexFormatShort2Normalized;
                        case DataType::UnsignedShortVector2: return MTLVertexFormatUShort2;
                        case DataType::UnsignedShortVector2Norm: return MTLVertexFormatUShort2Normalized;

                        case DataType::ShortVector3: return MTLVertexFormatShort3;
                        case DataType::ShortVector3Norm: return MTLVertexFormatShort3Normalized;
                        case DataType::UnsignedShortVector3: return MTLVertexFormatUShort3;
                        case DataType::UnsignedShortVector3Norm: return MTLVertexFormatUShort3Normalized;

                        case DataType::ShortVector4: return MTLVertexFormatShort4;
                        case DataType::ShortVector4Norm: return MTLVertexFormatShort4Normalized;
                        case DataType::UnsignedShortVector4: return MTLVertexFormatUShort4;
                        case DataType::UnsignedShortVector4Norm: return MTLVertexFormatUShort4Normalized;

                        case DataType::Integer: return MTLVertexFormatInt;
                        case DataType::UnsignedInteger: return MTLVertexFormatUInt;

                        case DataType::IntegerVector2: return MTLVertexFormatInt2;
                        case DataType::UnsignedIntegerVector2: return MTLVertexFormatUInt2;

                        case DataType::IntegerVector3: return MTLVertexFormatInt3;
                        case DataType::UnsignedIntegerVector3: return MTLVertexFormatUInt3;

                        case DataType::IntegerVector4: return MTLVertexFormatInt4;
                        case DataType::UnsignedIntegerVector4: return MTLVertexFormatUInt4;

                        case DataType::Float: return MTLVertexFormatFloat;
                        case DataType::FloatVector2: return MTLVertexFormatFloat2;
                        case DataType::FloatVector3: return MTLVertexFormatFloat3;
                        case DataType::FloatVector4: return MTLVertexFormatFloat4;
                        case DataType::FloatMatrix3: return MTLVertexFormatInvalid;
                        case DataType::FloatMatrix4: return MTLVertexFormatInvalid;

                        default: throw Error("Invalid data type");
                    }
                }
            }

            Shader::Shader(RenderDevice& initRenderDevice,
                           const std::vector<std::uint8_t>& fragmentShaderData,
                           const std::vector<std::uint8_t>& vertexShaderData,
                           const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                           const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                           const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                           const std::string& fragmentShaderFunction,
                           const std::string& vertexShaderFunction):
                RenderResource(initRenderDevice),
                vertexAttributes(initVertexAttributes),
                fragmentShaderConstantInfo(initFragmentShaderConstantInfo),
                vertexShaderConstantInfo(initVertexShaderConstantInfo)
            {
                std::uint32_t index = 0;
                NSUInteger offset = 0;

                vertexDescriptor = [[MTLVertexDescriptor alloc] init];

                for (const auto& vertexAttribute : RenderDevice::VERTEX_ATTRIBUTES)
                {
                    if (vertexAttributes.find(vertexAttribute.usage) != vertexAttributes.end())
                    {
                        const MTLVertexFormat vertexFormat = getVertexFormat(vertexAttribute.dataType);

                        if (vertexFormat == MTLVertexFormatInvalid)
                            throw Error("Invalid vertex format");

                        vertexDescriptor.get().attributes[index].format = vertexFormat;
                        vertexDescriptor.get().attributes[index].offset = offset;
                        vertexDescriptor.get().attributes[index].bufferIndex = 0;
                        ++index;
                    }

                    offset += getDataTypeSize(vertexAttribute.dataType);
                }

                vertexDescriptor.get().layouts[0].stride = offset;
                vertexDescriptor.get().layouts[0].stepRate = 1;
                vertexDescriptor.get().layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

                NSError* err;

                dispatch_data_t fragmentShaderDispatchData = dispatch_data_create(fragmentShaderData.data(), fragmentShaderData.size(), nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                Pointer<id<MTLLibrary>> fragmentShaderLibrary = [renderDevice.getDevice().get() newLibraryWithData:fragmentShaderDispatchData error:&err];
                dispatch_release(fragmentShaderDispatchData);

                if (!fragmentShaderLibrary || err != nil)
                    throw Error("Failed to load pixel shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));

                fragmentShader = [fragmentShaderLibrary.get() newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:fragmentShaderFunction.c_str()])];

                if (!fragmentShader || err != nil)
                    throw Error("Failed to get function from shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));

                if (!fragmentShaderConstantInfo.empty())
                {
                    fragmentShaderConstantLocations.reserve(fragmentShaderConstantInfo.size());

                    for (const auto& info : fragmentShaderConstantInfo)
                    {
                        const std::uint32_t size = getDataTypeSize(info.second);
                        fragmentShaderConstantLocations.emplace_back(fragmentShaderConstantSize, size);
                        fragmentShaderConstantSize += size;
                    }
                }

                dispatch_data_t vertexShaderDispatchData = dispatch_data_create(vertexShaderData.data(), vertexShaderData.size(), nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                Pointer<id<MTLLibrary>> vertexShaderLibrary = [renderDevice.getDevice().get() newLibraryWithData:vertexShaderDispatchData error:&err];
                dispatch_release(vertexShaderDispatchData);

                if (!vertexShaderLibrary || err != nil)
                    throw Error("Failed to load vertex shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));

                vertexShader = [vertexShaderLibrary.get() newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:vertexShaderFunction.c_str()])];

                if (!vertexShader || err != nil)
                    throw Error("Failed to get function from shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));

                if (!vertexShaderConstantInfo.empty())
                {
                    vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

                    for (const auto& info : vertexShaderConstantInfo)
                    {
                        const std::uint32_t size = getDataTypeSize(info.second);
                        vertexShaderConstantLocations.emplace_back(vertexShaderConstantSize, size);
                        vertexShaderConstantSize += size;
                    }
                }

#if TARGET_OS_IOS || TARGET_OS_TV
                constexpr std::uint32_t alignment = 16U; // 16 bytes on iOS and tvOS
#else
                constexpr std::uint32_t alignment = 256U; // 256 bytes on macOS
#endif

                // align the size of the buffer to alignment bytes
                fragmentShaderAlignment = (fragmentShaderConstantSize + alignment - 1U) & ~(alignment - 1U);
                vertexShaderAlignment = (vertexShaderConstantSize + alignment - 1U) & ~(alignment - 1U);
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
