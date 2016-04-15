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
    namespace video
    {
        Shader::Shader()
        {

        }

        Shader::~Shader()
        {

        }

        bool Shader::initFromFiles(const std::string& pixelShader,
                                   const std::string& vertexShader,
                                   uint32_t vertexAttributes,
                                   const std::string& pixelShaderFunction,
                                   const std::string& vertexShaderFunction)
        {
            _pixelShaderFilename = pixelShader;
            _vertexShaderFilename = vertexShader;

            std::ifstream pixelShaderFile(sharedEngine->getFileSystem()->getPath(pixelShader));

            if (!pixelShaderFile)
            {
                log("Failed to open pixel shader file %s", pixelShader.c_str());
                return false;
            }

            pixelShaderFile.seekg(0, std::ios::end);
            size_t pixelShaderSize = static_cast<size_t>(pixelShaderFile.tellg());
            pixelShaderFile.seekg(0, std::ios::beg);

            std::vector<char> pixelShaderBuffer(pixelShaderSize);

            pixelShaderFile.read(pixelShaderBuffer.data(), static_cast<std::streamsize>(pixelShaderSize));

            std::ifstream vertexShaderFile(sharedEngine->getFileSystem()->getPath(vertexShader));

            if (!vertexShaderFile)
            {
                log("Failed to open vertex shader file %s", pixelShader.c_str());
                return false;
            }

            std::string vertexShaderCode;

            vertexShaderFile.seekg(0, std::ios::end);
            size_t vertexShaderSize = static_cast<size_t>(vertexShaderFile.tellg());
            vertexShaderFile.seekg(0, std::ios::beg);

            std::vector<char> vertexShaderBuffer(vertexShaderSize);

            vertexShaderFile.read(vertexShaderBuffer.data(), static_cast<std::streamsize>(vertexShaderSize));

            return initFromBuffers(reinterpret_cast<const uint8_t*>(pixelShaderBuffer.data()), static_cast<uint32_t>(pixelShaderSize),
                                   reinterpret_cast<const uint8_t*>(vertexShaderBuffer.data()), static_cast<uint32_t>(vertexShaderSize), vertexAttributes,
                                   pixelShaderFunction, vertexShaderFunction);
        }

        bool Shader::initFromBuffers(const uint8_t* pixelShader,
                                     uint32_t pixelShaderSize,
                                     const uint8_t* vertexShader,
                                     uint32_t vertexShaderSize,
                                     uint32_t vertexAttributes,
                                     const std::string& pixelShaderFunction,
                                     const std::string& vertexShaderFunction)
        {
            OUZEL_UNUSED(pixelShader);
            OUZEL_UNUSED(pixelShaderSize);
            OUZEL_UNUSED(vertexShader);
            OUZEL_UNUSED(vertexShaderSize);
            OUZEL_UNUSED(pixelShaderFunction);
            OUZEL_UNUSED(vertexShaderFunction);

            _vertexAttributes = vertexAttributes;

            return  true;
        }

        bool Shader::setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            _pixelShaderConstantInfo = constantInfo;

            if (alignment)
            {
                _pixelShaderAlignment = alignment;
            }
            else
            {
                _pixelShaderAlignment = 0;

                for (const ConstantInfo& info : _pixelShaderConstantInfo)
                {
                    _pixelShaderAlignment += info.size;
                }
            }

            return true;
        }

        bool Shader::setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment)
        {
            _vertexShaderConstantInfo = constantInfo;

            if (alignment)
            {
                _vertexShaderAlignment = alignment;
            }
            else
            {
                _vertexShaderAlignment = 0;

                for (const ConstantInfo& info : _pixelShaderConstantInfo)
                {
                    _vertexShaderAlignment += info.size;
                }
            }

            return true;
        }

        bool Shader::setPixelShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            OUZEL_UNUSED(index);
            OUZEL_UNUSED(vectors);

            return true;
        }

        bool Shader::setPixelShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            OUZEL_UNUSED(index);
            OUZEL_UNUSED(vectors);

            return true;
        }

        bool Shader::setPixelShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            OUZEL_UNUSED(index);
            OUZEL_UNUSED(matrices);

            return true;
        }

        bool Shader::setVertexShaderConstant(uint32_t index, const std::vector<Vector3>& vectors)
        {
            OUZEL_UNUSED(index);
            OUZEL_UNUSED(vectors);

            return true;
        }

        bool Shader::setVertexShaderConstant(uint32_t index, const std::vector<Vector4>& vectors)
        {
            OUZEL_UNUSED(index);
            OUZEL_UNUSED(vectors);

            return true;
        }

        bool Shader::setVertexShaderConstant(uint32_t index, const std::vector<Matrix4>& matrices)
        {
            OUZEL_UNUSED(index);
            OUZEL_UNUSED(matrices);

            return true;
        }
    } // namespace video
} // namespace ouzel
