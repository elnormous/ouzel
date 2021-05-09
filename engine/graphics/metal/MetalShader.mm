// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <dispatch/data.h>
#include <TargetConditionals.h>
#include <algorithm>
#include "MetalShader.hpp"
#include "MetalError.hpp"
#include "MetalRenderDevice.hpp"

namespace ouzel::graphics::metal
{
    namespace
    {
        constexpr MTLVertexFormat getVertexFormat(DataType dataType)
        {
            switch (dataType)
            {
                case DataType::byte: return MTLVertexFormatInvalid;
                case DataType::byteNorm: return MTLVertexFormatInvalid;
                case DataType::unsignedByte: return MTLVertexFormatInvalid;
                case DataType::unsignedByteNorm: return MTLVertexFormatInvalid;

                case DataType::byteVector2: return MTLVertexFormatChar2;
                case DataType::byteVector2Norm: return MTLVertexFormatChar2Normalized;
                case DataType::unsignedByteVector2: return MTLVertexFormatUChar2;
                case DataType::unsignedByteVector2Norm: return MTLVertexFormatUChar2Normalized;

                case DataType::byteVector3: return MTLVertexFormatChar3;
                case DataType::byteVector3Norm: return MTLVertexFormatChar3Normalized;
                case DataType::unsignedByteVector3: return MTLVertexFormatUChar3;
                case DataType::unsignedByteVector3Norm: return MTLVertexFormatUChar3Normalized;

                case DataType::byteVector4: return MTLVertexFormatChar4;
                case DataType::byteVector4Norm: return MTLVertexFormatChar4Normalized;
                case DataType::unsignedByteVector4: return MTLVertexFormatUChar4;
                case DataType::unsignedByteVector4Norm: return MTLVertexFormatUChar4Normalized;

                case DataType::integer16: return MTLVertexFormatInvalid;
                case DataType::integer16Norm: return MTLVertexFormatInvalid;
                case DataType::unsignedInteger16: return MTLVertexFormatInvalid;
                case DataType::unsignedInteger16Norm: return MTLVertexFormatInvalid;

                case DataType::integer16Vector2: return MTLVertexFormatShort2;
                case DataType::integer16Vector2Norm: return MTLVertexFormatShort2Normalized;
                case DataType::unsignedInteger16Vector2: return MTLVertexFormatUShort2;
                case DataType::unsignedInteger16Vector2Norm: return MTLVertexFormatUShort2Normalized;

                case DataType::integer16Vector3: return MTLVertexFormatShort3;
                case DataType::integer16Vector3Norm: return MTLVertexFormatShort3Normalized;
                case DataType::unsignedInteger16Vector3: return MTLVertexFormatUShort3;
                case DataType::unsignedInteger16Vector3Norm: return MTLVertexFormatUShort3Normalized;

                case DataType::integer16Vector4: return MTLVertexFormatShort4;
                case DataType::integer16Vector4Norm: return MTLVertexFormatShort4Normalized;
                case DataType::unsignedInteger16Vector4: return MTLVertexFormatUShort4;
                case DataType::unsignedInteger16Vector4Norm: return MTLVertexFormatUShort4Normalized;

                case DataType::integer32: return MTLVertexFormatInt;
                case DataType::unsignedInteger32: return MTLVertexFormatUInt;

                case DataType::integer32Vector2: return MTLVertexFormatInt2;
                case DataType::unsignedInteger32Vector2: return MTLVertexFormatUInt2;

                case DataType::integer32Vector3: return MTLVertexFormatInt3;
                case DataType::unsignedInteger32Vector3: return MTLVertexFormatUInt3;

                case DataType::integer32Vector4: return MTLVertexFormatInt4;
                case DataType::unsignedInteger32Vector4: return MTLVertexFormatUInt4;

                case DataType::float32: return MTLVertexFormatFloat;
                case DataType::float32Vector2: return MTLVertexFormatFloat2;
                case DataType::float32Vector3: return MTLVertexFormatFloat3;
                case DataType::float32Vector4: return MTLVertexFormatFloat4;
                case DataType::float32Matrix3: return MTLVertexFormatInvalid;
                case DataType::float32Matrix4: return MTLVertexFormatInvalid;

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
        RenderResource{initRenderDevice},
        vertexAttributes{initVertexAttributes},
        fragmentShaderConstantInfo{initFragmentShaderConstantInfo},
        vertexShaderConstantInfo{initVertexShaderConstantInfo}
    {
        std::uint32_t index = 0;
        NSUInteger offset = 0;

        vertexDescriptor = [[MTLVertexDescriptor alloc] init];

        for (const auto& vertexAttribute : RenderDevice::vertexAttributes)
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
}

#endif
