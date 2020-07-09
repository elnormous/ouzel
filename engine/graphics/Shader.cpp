// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "Shader.hpp"
#include "Graphics.hpp"

namespace ouzel::graphics
{
    Shader::Shader(Renderer& initRenderer):
        resource(initRenderer.getDevice()->createResource())
    {
    }

    Shader::Shader(Renderer& initRenderer,
                   const std::vector<std::uint8_t>& initFragmentShader,
                   const std::vector<std::uint8_t>& initVertexShader,
                   const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                   const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                   const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                   const std::string& fragmentShaderFunction,
                   const std::string& vertexShaderFunction):
        resource(initRenderer.getDevice()->createResource()),
        vertexAttributes(initVertexAttributes)
    {
        initRenderer.addCommand(std::make_unique<InitShaderCommand>(resource,
                                                                    initFragmentShader,
                                                                    initVertexShader,
                                                                    initVertexAttributes,
                                                                    initFragmentShaderConstantInfo,
                                                                    initVertexShaderConstantInfo,
                                                                    fragmentShaderFunction,
                                                                    vertexShaderFunction));
    }
}
