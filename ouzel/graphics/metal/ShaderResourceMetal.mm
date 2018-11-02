// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <algorithm>
#include "ShaderResourceMetal.hpp"
#include "RenderDeviceMetal.hpp"
#include "files/FileSystem.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace graphics
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

        ShaderResourceMetal::ShaderResourceMetal(RenderDeviceMetal& renderDeviceMetal,
                                                 const std::vector<uint8_t>& fragmentShaderData,
                                                 const std::vector<uint8_t>& vertexShaderData,
                                                 const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                                                 const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                                                 const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                                 uint32_t newFragmentShaderDataAlignment,
                                                 uint32_t newVertexShaderDataAlignment,
                                                 const std::string& fragmentShaderFunction,
                                                 const std::string& vertexShaderFunction):
            RenderResourceMetal(renderDeviceMetal),
            vertexAttributes(newVertexAttributes),
            fragmentShaderConstantInfo(newFragmentShaderConstantInfo),
            vertexShaderConstantInfo(newVertexShaderConstantInfo)
        {
            if (newFragmentShaderDataAlignment)
                fragmentShaderAlignment = newFragmentShaderDataAlignment;
            else
            {
                fragmentShaderAlignment = 0;

                for (const Shader::ConstantInfo& info : newFragmentShaderConstantInfo)
                    fragmentShaderAlignment += info.size;
            }

            if (newVertexShaderDataAlignment)
                vertexShaderAlignment = newVertexShaderDataAlignment;
            else
            {
                vertexShaderAlignment = 0;

                for (const Shader::ConstantInfo& info : newVertexShaderConstantInfo)
                    vertexShaderAlignment += info.size;
            }

            uint32_t index = 0;
            NSUInteger offset = 0;

            vertexDescriptor = [MTLVertexDescriptor new];

            for (const Vertex::Attribute& vertexAttribute : Vertex::ATTRIBUTES)
            {
                if (vertexAttributes.find(vertexAttribute.usage) != vertexAttributes.end())
                {
                    MTLVertexFormat vertexFormat = getVertexFormat(vertexAttribute.dataType);

                    if (vertexFormat == MTLVertexFormatInvalid)
                        throw DataError("Invalid vertex format");

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
            id<MTLLibrary> fragmentShaderLibrary = [renderDeviceMetal.getDevice() newLibraryWithData:fragmentShaderDispatchData error:&err];
            dispatch_release(fragmentShaderDispatchData);

            if (!fragmentShaderLibrary || err != nil)
            {
                if (fragmentShaderLibrary) [fragmentShaderLibrary release];
                throw DataError("Failed to load pixel shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));
            }

            if (fragmentShader) [fragmentShader release];

            fragmentShader = [fragmentShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:fragmentShaderFunction.c_str()])];

            [fragmentShaderLibrary release];

            if (!fragmentShader || err != nil)
            {
                throw DataError("Failed to get function from shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));
            }

            if (!fragmentShaderConstantInfo.empty())
            {
                fragmentShaderConstantLocations.clear();
                fragmentShaderConstantLocations.reserve(fragmentShaderConstantInfo.size());

                fragmentShaderConstantSize = 0;

                for (const Shader::ConstantInfo& info : fragmentShaderConstantInfo)
                {
                    fragmentShaderConstantLocations.push_back({fragmentShaderConstantSize, info.size});
                    fragmentShaderConstantSize += info.size;
                }
            }

            dispatch_data_t vertexShaderDispatchData = dispatch_data_create(vertexShaderData.data(), vertexShaderData.size(), nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            id<MTLLibrary> vertexShaderLibrary = [renderDeviceMetal.getDevice() newLibraryWithData:vertexShaderDispatchData error:&err];
            dispatch_release(vertexShaderDispatchData);

            if (!vertexShaderLibrary || err != nil)
            {
                if (vertexShaderLibrary) [vertexShaderLibrary release];
                throw DataError("Failed to load vertex shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));
            }

            if (vertexShader) [vertexShader release];

            vertexShader = [vertexShaderLibrary newFunctionWithName:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:vertexShaderFunction.c_str()])];

            [vertexShaderLibrary release];

            if (!vertexShader || err != nil)
                throw DataError("Failed to get function from shader, " + std::string(err ? [err.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding] : "unknown error"));

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
        }

        ShaderResourceMetal::~ShaderResourceMetal()
        {
            if (vertexShader) [vertexShader release];
            if (fragmentShader) [fragmentShader release];
            if (vertexDescriptor) [vertexDescriptor release];
        }
    } // namespace graphics
} // namespace ouzel

#endif
