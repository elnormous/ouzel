// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Shader.h"
#include "ShaderResource.h"
#include "Renderer.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "files/FileSystem.h"

namespace ouzel
{
    namespace graphics
    {
        Shader::Shader()
        {
            resource = sharedEngine->getRenderer()->createShader();
        }

        Shader::~Shader()
        {
            if (sharedEngine && resource) sharedEngine->getRenderer()->deleteResource(resource);
        }

        bool Shader::initFromFiles(const std::string& newPixelShader,
                                   const std::string& newVertexShader,
                                   const std::vector<VertexAttribute>& newVertexAttributes,
                                   const std::vector<ConstantInfo>& newPixelShaderConstantInfo,
                                   const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                                   uint32_t newPixelShaderDataAlignment,
                                   uint32_t newVertexShaderDataAlignment,
                                   const std::string& newPixelShaderFunction,
                                   const std::string& newVertexShaderFunction)
        {
            pixelShaderFilename = newPixelShader;
            vertexShaderFilename = newVertexShader;

            std::vector<uint8_t> pixelShaderData;

            if (!sharedApplication->getFileSystem()->readFile(newPixelShader, pixelShaderData))
            {
                return false;
            }

            std::vector<uint8_t> vertexShaderData;

            if (!sharedApplication->getFileSystem()->readFile(newVertexShader, vertexShaderData))
            {
                return false;
            }

            if (!resource->initFromBuffers(pixelShaderData, vertexShaderData,
                                           newVertexAttributes,
                                           newPixelShaderConstantInfo,
                                           newVertexShaderConstantInfo,
                                           newPixelShaderDataAlignment,
                                           newVertexShaderDataAlignment,
                                           newPixelShaderFunction,
                                           newVertexShaderFunction))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Shader::initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                     const std::vector<uint8_t>& newVertexShader,
                                     const std::vector<VertexAttribute>& newVertexAttributes,
                                     const std::vector<Shader::ConstantInfo>& newPixelShaderConstantInfo,
                                     const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                     uint32_t newPixelShaderDataAlignment,
                                     uint32_t newVertexShaderDataAlignment,
                                     const std::string& newPixelShaderFunction,
                                     const std::string& newVertexShaderFunction)
        {
            pixelShaderFilename.clear();
            vertexShaderFilename.clear();

            if (!resource->initFromBuffers(newPixelShader, newVertexShader,
                                           newVertexAttributes,
                                           newPixelShaderConstantInfo,
                                           newVertexShaderConstantInfo,
                                           newPixelShaderDataAlignment,
                                           newVertexShaderDataAlignment,
                                           newPixelShaderFunction,
                                           newVertexShaderFunction))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return  true;
        }

        const std::vector<VertexAttribute>& Shader::getVertexAttributes() const
        {
            return resource->getVertexAttributes();
        }

        uint32_t Shader::getVertexSize() const
        {
            return resource->getVertexSize();
        }
    } // namespace graphics
} // namespace ouzel
