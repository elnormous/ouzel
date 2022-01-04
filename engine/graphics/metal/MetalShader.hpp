// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_METALSHADER_HPP
#define OUZEL_GRAPHICS_METALSHADER_HPP

#include <utility>

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#ifdef __OBJC__
#  import <Metal/Metal.h>
typedef id<MTLFunction> MTLFunctionPtr;
typedef MTLVertexDescriptor* MTLVertexDescriptorPtr;
#else
#  include <objc/objc.h>
using MTLFunctionPtr = id;
using MTLVertexDescriptorPtr = id;
#endif

#include "MetalRenderResource.hpp"
#include "../DataType.hpp"
#include "../Vertex.hpp"
#include "../../platform/objc/Pointer.hpp"

namespace ouzel::graphics::metal
{
    class RenderDevice;

    class Shader final: public RenderResource
    {
    public:
        Shader(RenderDevice& initRenderDevice,
               const std::vector<std::uint8_t>& fragmentShaderData,
               const std::vector<std::uint8_t>& vertexShaderData,
               const std::set<Vertex::Attribute::Semantic>& initVertexAttributes,
               const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
               const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
               const std::string& fragmentShaderFunction,
               const std::string& vertexShaderFunction);

        struct Location final
        {
            Location(std::uint32_t initOffset, std::uint32_t initSize):
                offset(initOffset), size(initSize)
            {
            }

            std::uint32_t offset;
            std::uint32_t size;
        };

        auto& getVertexAttributes() const noexcept { return vertexAttributes; }

        auto getFragmentShaderAlignment() const noexcept { return fragmentShaderAlignment; }
        auto getVertexShaderAlignment() const noexcept { return vertexShaderAlignment; }

        auto& getFragmentShaderConstantLocations() const noexcept { return fragmentShaderConstantLocations; }
        auto& getVertexShaderConstantLocations() const noexcept { return vertexShaderConstantLocations; }

        auto& getFragmentShader() const noexcept { return fragmentShader; }
        auto& getVertexShader() const noexcept { return vertexShader; }

        auto& getVertexDescriptor() const noexcept { return vertexDescriptor; }

        auto getFragmentShaderConstantBufferSize() const noexcept { return fragmentShaderConstantSize; }
        auto getVertexShaderConstantBufferSize() const noexcept { return vertexShaderConstantSize; }

    private:
        std::set<Vertex::Attribute::Semantic> vertexAttributes;

        std::vector<std::pair<std::string, DataType>> fragmentShaderConstantInfo;
        std::uint32_t fragmentShaderAlignment = 0;
        std::vector<std::pair<std::string, DataType>> vertexShaderConstantInfo;
        std::uint32_t vertexShaderAlignment = 0;

        platform::objc::Pointer<MTLFunctionPtr> fragmentShader;
        platform::objc::Pointer<MTLFunctionPtr> vertexShader;

        platform::objc::Pointer<MTLVertexDescriptorPtr> vertexDescriptor;

        std::vector<Location> fragmentShaderConstantLocations;
        std::uint32_t fragmentShaderConstantSize = 0;
        std::vector<Location> vertexShaderConstantLocations;
        std::uint32_t vertexShaderConstantSize = 0;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALSHADER_HPP
