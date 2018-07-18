// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Shader.hpp"
#include "ShaderResource.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "core/Engine.hpp"
#include "files/FileSystem.hpp"

namespace ouzel
{
    namespace graphics
    {
        Shader::Shader(Renderer* initRenderer):
            renderer(initRenderer)
        {
            resource = renderer->getDevice()->createShader();
        }

        Shader::~Shader()
        {
            if (resource) renderer->getDevice()->deleteResource(resource);
        }

        void Shader::init(const std::string& newFragmentShader,
                          const std::string& newVertexShader,
                          const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                          const std::vector<ConstantInfo>& newFragmentShaderConstantInfo,
                          const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                          uint32_t newFragmentShaderDataAlignment,
                          uint32_t newVertexShaderDataAlignment,
                          const std::string& newFragmentShaderFunction,
                          const std::string& newVertexShaderFunction)
        {
            vertexAttributes = newVertexAttributes;

            fragmentShaderFilename = newFragmentShader;
            vertexShaderFilename = newVertexShader;

            std::vector<uint8_t> fragmentShaderData = engine->getFileSystem()->readFile(newFragmentShader);
            std::vector<uint8_t> vertexShaderData = engine->getFileSystem()->readFile(newVertexShader);

            RenderDevice* renderDevice = renderer->getDevice();

            renderDevice->addCommand(InitShaderCommand(resource,
                                                       fragmentShaderData,
                                                       vertexShaderData,
                                                       newVertexAttributes,
                                                       newFragmentShaderConstantInfo,
                                                       newVertexShaderConstantInfo,
                                                       newFragmentShaderDataAlignment,
                                                       newVertexShaderDataAlignment,
                                                       newFragmentShaderFunction,
                                                       newVertexShaderFunction));
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

            RenderDevice* renderDevice = renderer->getDevice();

            renderDevice->addCommand(InitShaderCommand(resource,
                                                       newFragmentShader,
                                                       newVertexShader,
                                                       newVertexAttributes,
                                                       newFragmentShaderConstantInfo,
                                                       newVertexShaderConstantInfo,
                                                       newFragmentShaderDataAlignment,
                                                       newVertexShaderDataAlignment,
                                                       newFragmentShaderFunction,
                                                       newVertexShaderFunction));
        }

        const std::set<Vertex::Attribute::Usage>& Shader::getVertexAttributes() const
        {
            return vertexAttributes;
        }
    } // namespace graphics
} // namespace ouzel
