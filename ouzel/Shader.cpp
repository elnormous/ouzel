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

            std::ifstream pixelShaderFile(sharedEngine->getFileSystem()->getPath(newPixelShader));

            if (!pixelShaderFile)
            {
                log("Failed to open pixel shader file %s", newPixelShader.c_str());
                return false;
            }

            pixelShaderFile.seekg(0, std::ios::end);
            size_t pixelShaderSize = static_cast<size_t>(pixelShaderFile.tellg());
            pixelShaderFile.seekg(0, std::ios::beg);

            std::vector<char> pixelShaderBuffer(pixelShaderSize);

            pixelShaderFile.read(pixelShaderBuffer.data(), static_cast<std::streamsize>(pixelShaderSize));

            std::ifstream vertexShaderFile(sharedEngine->getFileSystem()->getPath(newVertexShader));

            if (!vertexShaderFile)
            {
                log("Failed to open vertex shader file %s", newVertexShader.c_str());
                return false;
            }

            std::string vertexShaderCode;

            vertexShaderFile.seekg(0, std::ios::end);
            size_t vertexShaderSize = static_cast<size_t>(vertexShaderFile.tellg());
            vertexShaderFile.seekg(0, std::ios::beg);

            std::vector<char> vertexShaderBuffer(vertexShaderSize);

            vertexShaderFile.read(vertexShaderBuffer.data(), static_cast<std::streamsize>(vertexShaderSize));

            return initFromBuffers(reinterpret_cast<const uint8_t*>(pixelShaderBuffer.data()), static_cast<uint32_t>(pixelShaderSize),
                                   reinterpret_cast<const uint8_t*>(vertexShaderBuffer.data()), static_cast<uint32_t>(vertexShaderSize), newVertexAttributes,
                                   pixelShaderFunction, vertexShaderFunction);
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

            pixelShaderFilename = std::string();
            vertexShaderFilename = std::string();
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
