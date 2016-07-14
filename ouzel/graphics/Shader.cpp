// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "Shader.h"
#include "core/Engine.h"
#include "Renderer.h"
#include "files/FileSystem.h"
#include "utils/Utils.h"

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

        void Shader::free()
        {
            ready = false;
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

        bool Shader::initFromBuffers(const uint8_t*,
                                     uint32_t,
                                     const uint8_t*,
                                     uint32_t,
                                     uint32_t newVertexAttributes,
                                     const std::string&,
                                     const std::string&)
        {
            vertexAttributes = newVertexAttributes;

            ready = true;

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

        bool Shader::setPixelShaderConstant(uint32_t, uint32_t, uint32_t, const float*)
        {
            return true;
        }

        bool Shader::setVertexShaderConstant(uint32_t, uint32_t, uint32_t, const float*)
        {
            return true;
        }
    } // namespace graphics
} // namespace ouzel
