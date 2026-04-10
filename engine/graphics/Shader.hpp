#ifndef FLUXION_GRAPHICS_SHADER_HPP
#define FLUXION_GRAPHICS_SHADER_HPP

#include <set>
#include <string>
#include <vector>
#include <utility>
#include "RenderDevice.hpp"
#include "DataType.hpp"
#include "Vertex.hpp"

namespace fluxion::graphics
{
    class Graphics;

    class Shader final
    {
    public:
        Shader() = default;

        explicit Shader(Graphics& initGraphics);
        Shader(Graphics& initGraphics,
               const std::vector<std::uint8_t>& initFragmentShader,
               const std::vector<std::uint8_t>& initVertexShader,
               const std::set<Vertex::Attribute::Semantic>& initVertexAttributes,
               const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
               const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
               const std::string& fragmentShaderFunction = "",
               const std::string& vertexShaderFunction = "");

        auto& getResource() const noexcept { return resource; }

        auto& getVertexAttributes() const noexcept { return vertexAttributes; }

    private:
        RenderDevice::Resource resource;

        std::set<Vertex::Attribute::Semantic> vertexAttributes;
    };
}

#endif // FLUXION_GRAPHICS_SHADER_HPP
