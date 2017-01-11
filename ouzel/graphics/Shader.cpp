// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Shader.h"
#include "Renderer.h"
#include "core/Engine.h"
#include "core/Application.h"
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
            data.pixelShaderData.clear();
            data.vertexShaderData.clear();
            data.pixelShaderFunction.clear();
            data.vertexShaderFunction.clear();
            data.pixelShaderConstantInfo.clear();
            data.vertexShaderConstantInfo.clear();

            currentData.pixelShaderData.clear();
            currentData.vertexShaderData.clear();
            currentData.pixelShaderFunction.clear();
            currentData.vertexShaderFunction.clear();
            currentData.pixelShaderConstantInfo.clear();
            currentData.vertexShaderConstantInfo.clear();
        }

        bool Shader::initFromFiles(const std::string& newPixelShader,
                                   const std::string& newVertexShader,
                                   uint32_t newVertexAttributes,
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

            return initFromBuffers(pixelShaderData, vertexShaderData,
                                   newVertexAttributes,
                                   newPixelShaderConstantInfo, newVertexShaderConstantInfo,
                                   newPixelShaderDataAlignment, newVertexShaderDataAlignment,
                                   newPixelShaderFunction, newVertexShaderFunction);
        }

        bool Shader::initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                     const std::vector<uint8_t>& newVertexShader,
                                     uint32_t newVertexAttributes,
                                     const std::vector<ConstantInfo>& newPixelShaderConstantInfo,
                                     const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                                     uint32_t newPixelShaderDataAlignment,
                                     uint32_t newVertexShaderDataAlignment,
                                     const std::string& newPixelShaderFunction,
                                     const std::string& newVertexShaderFunction)
        {
            data.pixelShaderData = newPixelShader;
            data.vertexShaderData = newVertexShader;
            data.vertexAttributes = newVertexAttributes;
            data.pixelShaderConstantInfo = newPixelShaderConstantInfo;
            data.vertexShaderConstantInfo = newVertexShaderConstantInfo;

            if (newPixelShaderDataAlignment)
            {
                data.pixelShaderAlignment = newPixelShaderDataAlignment;
            }
            else
            {
                data.pixelShaderAlignment = 0;

                for (const ConstantInfo& info : newPixelShaderConstantInfo)
                {
                    data.pixelShaderAlignment += info.size;
                }
            }

            if (newVertexShaderDataAlignment)
            {
                data.vertexShaderAlignment = newVertexShaderDataAlignment;
            }
            else
            {
                data.vertexShaderAlignment = 0;

                for (const ConstantInfo& info : newVertexShaderConstantInfo)
                {
                    data.vertexShaderAlignment += info.size;
                }
            }

            data.pixelShaderFunction = newPixelShaderFunction;
            data.vertexShaderFunction = newVertexShaderFunction;

            update();

            return  true;
        }

        void Shader::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            currentData.vertexAttributes = data.vertexAttributes;
            currentData.pixelShaderAlignment = data.pixelShaderAlignment;
            currentData.vertexShaderAlignment = data.vertexShaderAlignment;

            currentData.pixelShaderData = std::move(data.pixelShaderData);
            currentData.vertexShaderData = std::move(data.vertexShaderData);
            currentData.pixelShaderConstantInfo = std::move(data.pixelShaderConstantInfo);
            currentData.vertexShaderConstantInfo = std::move(data.vertexShaderConstantInfo);
            currentData.pixelShaderFunction = std::move(data.pixelShaderFunction);
            currentData.vertexShaderFunction = std::move(data.vertexShaderFunction);

            dirty = true;
        }

        bool Shader::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dirty = false;
            uploadData = std::move(currentData);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
