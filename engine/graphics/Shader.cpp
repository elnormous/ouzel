// Ouzel by Elviss Strazdins

#include "Shader.hpp"
#include "Graphics.hpp"

namespace ouzel::graphics
{
    Shader::Shader(Graphics& initGraphics):
        resource(*initGraphics.getDevice())
    {
    }

    Shader::Shader(Graphics& initGraphics,
                   const std::vector<std::uint8_t>& initFragmentShader,
                   const std::vector<std::uint8_t>& initVertexShader,
                   const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                   const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                   const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                   const std::string& fragmentShaderFunction,
                   const std::string& vertexShaderFunction):
        resource(*initGraphics.getDevice()),
        vertexAttributes(initVertexAttributes)
    {
        initGraphics.addCommand(std::make_unique<InitShaderCommand>(resource,
                                                                    initFragmentShader,
                                                                    initVertexShader,
                                                                    initVertexAttributes,
                                                                    initFragmentShaderConstantInfo,
                                                                    initVertexShaderConstantInfo,
                                                                    fragmentShaderFunction,
                                                                    vertexShaderFunction));
    }
}
