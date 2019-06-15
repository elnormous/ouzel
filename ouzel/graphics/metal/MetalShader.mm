// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <algorithm>
#include <stdexcept>
#include "MetalShader.hpp"
#include "MetalRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            static MTLVertexFormat getVertexFormat(DataType dataType)
            {
                switch (dataType)
                {
                    case DataType::BYTE: return MTLVertexFormatInvalid;
                    case DataType::BYTE_NORM: return MTLVertexFormatInvalid;
                    case DataType::UNSIGNED_BYTE: return MTLVertexFormatInvalid;
                    case DataType::UNSIGNED_BYTE_NORM: return MTLVertexFormatInvalid;

                    case DataType::BYTE_VECTOR2: return MTLVertexFormatChar2;
                    case DataType::BYTE_VECTOR2_NORM: return MTLVertexFormatChar2Normalized;
                    case DataType::UNSIGNED_BYTE_VECTOR2: return MTLVertexFormatUChar2;
                    case DataType::UNSIGNED_BYTE_VECTOR2_NORM: return MTLVertexFormatUChar2Normalized;

                    case DataType::BYTE_VECTOR3: return MTLVertexFormatChar3;
                    case DataType::BYTE_VECTOR3_NORM: return MTLVertexFormatChar3Normalized;
                    case DataType::UNSIGNED_BYTE_VECTOR3: return MTLVertexFormatUChar3;
                    case DataType::UNSIGNED_BYTE_VECTOR3_NORM: return MTLVertexFormatUChar3Normalized;

                    case DataType::BYTE_VECTOR4: return MTLVertexFormatChar4;
                    case DataType::BYTE_VECTOR4_NORM: return MTLVertexFormatChar4Normalized;
                    case DataType::UNSIGNED_BYTE_VECTOR4: return MTLVertexFormatUChar4;
                    case DataType::UNSIGNED_BYTE_VECTOR4_NORM: return MTLVertexFormatUChar4Normalized;

                    case DataType::SHORT: return MTLVertexFormatInvalid;
                    case DataType::SHORT_NORM: return MTLVertexFormatInvalid;
                    case DataType::UNSIGNED_SHORT: return MTLVertexFormatInvalid;
                    case DataType::UNSIGNED_SHORT_NORM: return MTLVertexFormatInvalid;

                    case DataType::SHORT_VECTOR2: return MTLVertexFormatShort2;
                    case DataType::SHORT_VECTOR2_NORM: return MTLVertexFormatShort2Normalized;
                    case DataType::UNSIGNED_SHORT_VECTOR2: return MTLVertexFormatUShort2;
                    case DataType::UNSIGNED_SHORT_VECTOR2_NORM: return MTLVertexFormatUShort2Normalized;

                    case DataType::SHORT_VECTOR3: return MTLVertexFormatShort3;
                    case DataType::SHORT_VECTOR3_NORM: return MTLVertexFormatShort3Normalized;
                    case DataType::UNSIGNED_SHORT_VECTOR3: return MTLVertexFormatUShort3;
                    case DataType::UNSIGNED_SHORT_VECTOR3_NORM: return MTLVertexFormatUShort3Normalized;

                    case DataType::SHORT_VECTOR4: return MTLVertexFormatShort4;
                    case DataType::SHORT_VECTOR4_NORM: return MTLVertexFormatShort4Normalized;
                    case DataType::UNSIGNED_SHORT_VECTOR4: return MTLVertexFormatUShort4;
                    case DataType::UNSIGNED_SHORT_VECTOR4_NORM: return MTLVertexFormatUShort4Normalized;

                    case DataType::INTEGER: return MTLVertexFormatInt;
                    case DataType::UNSIGNED_INTEGER: return MTLVertexFormatUInt;

                    case DataType::INTEGER_VECTOR2: return MTLVertexFormatInt2;
                    case DataType::UNSIGNED_INTEGER_VECTOR2: return MTLVertexFormatUInt2;

                    case DataType::INTEGER_VECTOR3: return MTLVertexFormatInt3;
                    case DataType::UNSIGNED_INTEGER_VECTOR3: return MTLVertexFormatUInt3;

                    case DataType::INTEGER_VECTOR4: return MTLVertexFormatInt4;
                    case DataType::UNSIGNED_INTEGER_VECTOR4: return MTLVertexFormatUInt4;

                    case DataType::FLOAT: return MTLVertexFormatFloat;
                    case DataType::FLOAT_VECTOR2: return MTLVertexFormatFloat2;
                    case DataType::FLOAT_VECTOR3: return MTLVertexFormatFloat3;
                    case DataType::FLOAT_VECTOR4: return MTLVertexFormatFloat4;
                    case DataType::FLOAT_MATRIX3: return MTLVertexFormatInvalid;
                    case DataType::FLOAT_MATRIX4: return MTLVertexFormatInvalid;

                    case DataType::NONE: return MTLVertexFormatInvalid;
                }

                return MTLVertexFormatInvalid;
            }

            Shader::Shader(RenderDevice& renderDevice,
                           const std::vector<uint8_t>& fragmentShaderData,
                           const std::vector<uint8_t>& vertexShaderData,
                           const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                           const std::vector<graphics::Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                           const std::vector<graphics::Shader::ConstantInfo>& newVertexShaderConstantInfo,
                           uint32_t newFragmentShaderDataAlignment,
                           uint32_t newVertexShaderDataAlignment,
                           const std::string& fragmentShaderFunction,
                           const std::string& vertexShaderFunction):
                RenderResource(renderDevice),
                vertexAttributes(newVertexAttributes),
                fragmentShaderConstantInfo(newFragmentShaderConstantInfo),
                vertexShaderConstantInfo(newVertexShaderConstantInfo)
            {
                if (newFragmentShaderDataAlignment)
                    fragmentShaderAlignment = newFragmentShaderDataAlignment;
                else
                {
                    fragmentShaderAlignment = 0;

                    for (const graphics::Shader::ConstantInfo& info : newFragmentShaderConstantInfo)
                        fragmentShaderAlignment += getDataTypeSize(info.dataType);
                }

                if (newVertexShaderDataAlignment)
                    vertexShaderAlignment = newVertexShaderDataAlignment;
                else
                {
                    vertexShaderAlignment = 0;

                    for (const graphics::Shader::ConstantInfo& info : newVertexShaderConstantInfo)
                        vertexShaderAlignment += getDataTypeSize(info.dataType);
                }

                uint32_t index = 0;
                NSUInteger offset = 0;

                vertexDescriptor = [MTLVertexDescriptor new];

                for (const Vertex::Attribute& vertexAttribute : RenderDevice::VERTEX_ATTRIBUTES)
                {
                    if (vertexAttributes.find(vertexAttribute.usage) != vertexAttributes.end())
                    {
                        MTLVertexFormat vertexFormat = getVertexFormat(vertexAttribute.dataType);

                        if (vertexFormat == MTLVertexFormatInvalid)
                            throw std::runtime_error("Invalid vertex format");

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

                dispatch_data_t fragmentShaderDispatchData = dispatch_data_create(fragmentShaderData.data(), fragmentShaderData.size(), nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                id<MTLLibrary> fragmentShaderLibrary = [renderDevice.getDevice() newLibraryWithData:fragmentShaderDispatchData error:&err];
                dispatch_release(fragmentShaderDispatchData);

                if (!fragmentShaderLibrary || err != nil)
                {
                    if (fragmentShaderLibrary) [fragmentShaderLibrary release];
                    throw std::runtime_error("Failed to load pixel shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));
                }

                if (fragmentShader) [fragmentShader release];

                fragmentShader = [fragmentShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:fragmentShaderFunction.c_str()])];

                [fragmentShaderLibrary release];

                if (!fragmentShader || err != nil)
                {
                    throw std::runtime_error("Failed to get function from shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));
                }

                if (!fragmentShaderConstantInfo.empty())
                {
                    fragmentShaderConstantLocations.clear();
                    fragmentShaderConstantLocations.reserve(fragmentShaderConstantInfo.size());

                    fragmentShaderConstantSize = 0;

                    for (const graphics::Shader::ConstantInfo& info : fragmentShaderConstantInfo)
                    {
                        uint32_t size = getDataTypeSize(info.dataType);
                        fragmentShaderConstantLocations.push_back({fragmentShaderConstantSize, size});
                        fragmentShaderConstantSize += size;
                    }
                }

                dispatch_data_t vertexShaderDispatchData = dispatch_data_create(vertexShaderData.data(), vertexShaderData.size(), nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                id<MTLLibrary> vertexShaderLibrary = [renderDevice.getDevice() newLibraryWithData:vertexShaderDispatchData error:&err];
                dispatch_release(vertexShaderDispatchData);

                if (!vertexShaderLibrary || err != nil)
                {
                    if (vertexShaderLibrary) [vertexShaderLibrary release];
                    throw std::runtime_error("Failed to load vertex shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));
                }

                if (vertexShader) [vertexShader release];

                vertexShader = [vertexShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:vertexShaderFunction.c_str()])];

                [vertexShaderLibrary release];

                if (!vertexShader || err != nil)
                    throw std::runtime_error("Failed to get function from shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));

                if (!vertexShaderConstantInfo.empty())
                {
                    vertexShaderConstantLocations.clear();
                    vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

                    vertexShaderConstantSize = 0;

                    for (const graphics::Shader::ConstantInfo& info : vertexShaderConstantInfo)
                    {
                        uint32_t size = getDataTypeSize(info.dataType);
                        vertexShaderConstantLocations.push_back({vertexShaderConstantSize, size});
                        vertexShaderConstantSize += size;
                    }
                }
            }

            Shader::~Shader()
            {
                if (vertexShader) [vertexShader release];
                if (fragmentShader) [fragmentShader release];
                if (vertexDescriptor) [vertexDescriptor release];
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
