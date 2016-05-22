// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "Shader.h"
#include "Engine.h"
#include "Renderer.h"
#include "FileSystem.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        Shader::Shader()
        {

        }

        Shader::~Shader()
        {

        }

        bool Shader::initFromFiles(const std::string& newPixelShader,
                                   const std::string& newVertexShader,
                                   uint32_t newVertexAttributes,
                                   const std::string& pixelShaderFunction,
                                   const std::string& vertexShaderFunction)
        {
            pixelShaderFilename = newPixelShader;
            vertexShaderFilename = newVertexShader;

            std::vector<uint8_t> pixelShaderData;

            if (!sharedEngine->getFileSystem()->loadFile(newPixelShader, pixelShaderData))
            {
                return false;
            }

            std::vector<uint8_t> vertexShaderData;

            if (!sharedEngine->getFileSystem()->loadFile(newVertexShader, vertexShaderData))
            {
                return false;
            }

            return initFromBuffers(reinterpret_cast<const uint8_t*>(pixelShaderData.data()), static_cast<uint32_t>(pixelShaderData.size()),
                                   reinterpret_cast<const uint8_t*>(vertexShaderData.data()), static_cast<uint32_t>(vertexShaderData.size()),
                                   newVertexAttributes, pixelShaderFunction, vertexShaderFunction);
        }

        bool Shader::initFromBuffers(const uint8_t* newPixelShader,
                                     uint32_t newPixelShaderSize,
                                     const uint8_t* newVertexShader,
                                     uint32_t newVertexShaderSize,
                                     uint32_t newVertexAttributes,
                                     const std::string& pixelShaderFunction,
                                     const std::string& vertexShaderFunction)
        {
            OUZEL_UNUSED(newPixelShader);
            OUZEL_UNUSED(newPixelShaderSize);
            OUZEL_UNUSED(newVertexShader);
            OUZEL_UNUSED(newVertexShaderSize);
            OUZEL_UNUSED(pixelShaderFunction);
            OUZEL_UNUSED(vertexShaderFunction);

            vertexAttributes = newVertexAttributes;

            return  true;
        }

        bool Shader::setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            pixelShaderConstantInfo = constantInfo;

            if (alignment)
            {
                pixelShaderAlignment = alignment;
            }
            else
            {
                pixelShaderAlignment = 0;

                for (const ConstantInfo& info : pixelShaderConstantInfo)
                {
                    pixelShaderAlignment += info.size;
                }
            }

            return true;
        }

        bool Shader::setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            vertexShaderConstantInfo = constantInfo;

            if (alignment)
            {
                vertexShaderAlignment = alignment;
            }
            else
            {
                vertexShaderAlignment = 0;

                for (const ConstantInfo& info : pixelShaderConstantInfo)
                {
                    vertexShaderAlignment += info.size;
                }
            }

            return true;
        }

        bool Shader::setPixelShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value)
        {
            OUZEL_UNUSED(index);
            OUZEL_UNUSED(size);
            OUZEL_UNUSED(count);
            OUZEL_UNUSED(value);

            return true;
        }

        bool Shader::setVertexShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value)
        {
            OUZEL_UNUSED(index);
            OUZEL_UNUSED(size);
            OUZEL_UNUSED(count);
            OUZEL_UNUSED(value);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
