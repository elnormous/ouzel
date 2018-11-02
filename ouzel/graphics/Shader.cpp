// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Shader.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        Shader::Shader(Renderer& initRenderer):
            renderer(initRenderer)
        {
            resource = renderer.getDevice()->getResourceId();
        }

        Shader::~Shader()
        {
            if (resource)
            {
                RenderDevice* renderDevice = renderer.getDevice();
                renderDevice->addCommand(std::unique_ptr<Command>(new DeleteShaderCommand(resource)));
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
                          const std::string& newFragmentShaderFunction,
                          const std::string& newVertexShaderFunction)
        {
            vertexAttributes = newVertexAttributes;
            fragmentShaderFilename.clear();
            vertexShaderFilename.clear();

            RenderDevice* renderDevice = renderer.getDevice();

            renderDevice->addCommand(std::unique_ptr<Command>(new InitShaderCommand(resource,
                                                                                    newFragmentShader,
                                                                                    newVertexShader,
                                                                                    newVertexAttributes,
                                                                                    newFragmentShaderConstantInfo,
                                                                                    newVertexShaderConstantInfo,
                                                                                    newFragmentShaderDataAlignment,
                                                                                    newVertexShaderDataAlignment,
                                                                                    newFragmentShaderFunction,
                                                                                    newVertexShaderFunction)));
        }

        const std::set<Vertex::Attribute::Usage>& Shader::getVertexAttributes() const
        {
            return vertexAttributes;
        }
    } // namespace graphics
} // namespace ouzel
