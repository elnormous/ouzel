// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Shader.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        Shader::Shader(Renderer& initRenderer):
            resource(*initRenderer.getDevice())
        {
        }

        Shader::Shader(Renderer& initRenderer,
                       const std::vector<uint8_t>& initFragmentShader,
                       const std::vector<uint8_t>& initVertexShader,
                       const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                       const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                       const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                       uint32_t initFragmentShaderDataAlignment,
                       uint32_t initVertexShaderDataAlignment,
                       const std::string& fragmentShaderFunction,
                       const std::string& vertexShaderFunction):
            resource(*initRenderer.getDevice()),
            vertexAttributes(initVertexAttributes)
        {
            initRenderer.addCommand(std::unique_ptr<Command>(new InitShaderCommand(resource,
                                                                                   initFragmentShader,
                                                                                   initVertexShader,
                                                                                   initVertexAttributes,
                                                                                   initFragmentShaderConstantInfo,
                                                                                   initVertexShaderConstantInfo,
                                                                                   initFragmentShaderDataAlignment,
                                                                                   initVertexShaderDataAlignment,
                                                                                   fragmentShaderFunction,
                                                                                   vertexShaderFunction)));
        }

        const std::set<Vertex::Attribute::Usage>& Shader::getVertexAttributes() const
        {
            return vertexAttributes;
        }
    } // namespace graphics
} // namespace ouzel
