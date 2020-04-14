// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_SHADER_HPP
#define OUZEL_GRAPHICS_SHADER_HPP

#include <set>
#include <string>
#include <vector>
#include <utility>
#include "RenderDevice.hpp"
#include "DataType.hpp"
#include "Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Shader final
        {
        public:
            Shader() = default;

            explicit Shader(Renderer& initRenderer);
            Shader(Renderer& initRenderer,
                   const std::vector<std::uint8_t>& initFragmentShader,
                   const std::vector<std::uint8_t>& initVertexShader,
                   const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                   const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                   const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                   const std::string& fragmentShaderFunction = "",
                   const std::string& vertexShaderFunction = "");

            inline auto& getResource() const noexcept { return resource; }

            inline auto& getVertexAttributes() const noexcept { return vertexAttributes; }

        private:
            RenderDevice::Resource resource;

            std::set<Vertex::Attribute::Usage> vertexAttributes;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_SHADER_HPP
