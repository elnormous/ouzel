// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Shader.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        Shader::Shader(Renderer& initRenderer):
            renderer(&initRenderer),
            resource(renderer->getDevice()->getResourceId())
        {
        }

        Shader::Shader(Renderer& initRenderer,
                       const std::vector<uint8_t>& initFragmentShader,
                       const std::vector<uint8_t>& initVertexShader,
                       const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                       const std::vector<ConstantInfo>& initFragmentShaderConstantInfo,
                       const std::vector<ConstantInfo>& initVertexShaderConstantInfo,
                       uint32_t initFragmentShaderDataAlignment,
                       uint32_t initVertexShaderDataAlignment,
                       const std::string& fragmentShaderFunction,
                       const std::string& vertexShaderFunction):
            renderer(&initRenderer),
            resource(renderer->getDevice()->getResourceId()),
            vertexAttributes(initVertexAttributes)
        {
            renderer->addCommand(std::unique_ptr<Command>(new InitShaderCommand(resource,
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

        Shader::~Shader()
        {
            if (renderer && resource)
            {
                renderer->addCommand(std::unique_ptr<Command>(new DeleteResourceCommand(resource)));
                RenderDevice* renderDevice = renderer->getDevice();
                renderDevice->deleteResourceId(resource);
            }
        }

        void Shader::init(const std::vector<uint8_t>& newFragmentShader,
                          const std::vector<uint8_t>& newVertexShader,
                          const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                          const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                          const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                          uint32_t newFragmentShaderDataAlignment,
                          uint32_t newVertexShaderDataAlignment,
                          const std::string& fragmentShaderFunction,
                          const std::string& vertexShaderFunction)
        {
            vertexAttributes = newVertexAttributes;

            if (renderer && resource)
                renderer->addCommand(std::unique_ptr<Command>(new InitShaderCommand(resource,
                                                                                    newFragmentShader,
                                                                                    newVertexShader,
                                                                                    newVertexAttributes,
                                                                                    newFragmentShaderConstantInfo,
                                                                                    newVertexShaderConstantInfo,
                                                                                    newFragmentShaderDataAlignment,
                                                                                    newVertexShaderDataAlignment,
                                                                                    fragmentShaderFunction,
                                                                                    vertexShaderFunction)));
        }

        const std::set<Vertex::Attribute::Usage>& Shader::getVertexAttributes() const
        {
            return vertexAttributes;
        }
    } // namespace graphics
} // namespace ouzel
