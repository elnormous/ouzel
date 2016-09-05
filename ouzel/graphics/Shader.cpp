// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include "Shader.h"
#include "core/Engine.h"
#include "core/Application.h"
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
            data.pixelShaderData.clear();
            data.vertexShaderData.clear();
            data.pixelShaderFunction.clear();
            data.vertexShaderFunction.clear();
            data.pixelShaderConstantInfo.clear();
            data.vertexShaderConstantInfo.clear();

            uploadData.pixelShaderData.clear();
            uploadData.vertexShaderData.clear();
            uploadData.pixelShaderFunction.clear();
            uploadData.vertexShaderFunction.clear();
            uploadData.pixelShaderConstantInfo.clear();
            uploadData.vertexShaderConstantInfo.clear();
            
            ready = false;
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

            if (!sharedApplication->getFileSystem()->loadFile(newPixelShader, pixelShaderData))
            {
                return false;
            }

            std::vector<uint8_t> vertexShaderData;

            if (!sharedApplication->getFileSystem()->loadFile(newVertexShader, vertexShaderData))
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

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return  true;
        }

        bool Shader::update()
        {
            uploadData.vertexAttributes = data.vertexAttributes;
            uploadData.pixelShaderAlignment = data.pixelShaderAlignment;
            uploadData.vertexShaderAlignment = data.vertexShaderAlignment;

            if (!data.pixelShaderData.empty())
            {
                uploadData.pixelShaderData = std::move(data.pixelShaderData);
            }

            if (!data.vertexShaderData.empty())
            {
                uploadData.vertexShaderData = std::move(data.vertexShaderData);
            }

            if (!data.pixelShaderConstantInfo.empty())
            {
                uploadData.pixelShaderConstantInfo = std::move(data.pixelShaderConstantInfo);
            }

            if (!data.vertexShaderConstantInfo.empty())
            {
                uploadData.vertexShaderConstantInfo = std::move(data.vertexShaderConstantInfo);
            }

            if (!data.pixelShaderFunction.empty())
            {
                uploadData.pixelShaderFunction = std::move(data.pixelShaderFunction);
            }

            if (!data.vertexShaderFunction.empty())
            {
                uploadData.vertexShaderFunction = std::move(data.vertexShaderFunction);
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
