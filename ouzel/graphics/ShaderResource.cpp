// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ShaderResource.h"

namespace ouzel
{
    namespace graphics
    {
        ShaderResource::ShaderResource()
        {
        }

        ShaderResource::~ShaderResource()
        {
        }

        bool ShaderResource::initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                             const std::vector<uint8_t>& newVertexShader,
                                             uint32_t newVertexAttributes,
                                             const std::vector<Shader::ConstantInfo>& newPixelShaderConstantInfo,
                                             const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                             uint32_t newPixelShaderDataAlignment,
                                             uint32_t newVertexShaderDataAlignment,
                                             const std::string& newPixelShaderFunction,
                                             const std::string& newVertexShaderFunction)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            pendingData.pixelShaderData = newPixelShader;
            pendingData.vertexShaderData = newVertexShader;
            pendingData.vertexAttributes = newVertexAttributes;
            pendingData.pixelShaderConstantInfo = newPixelShaderConstantInfo;
            pendingData.vertexShaderConstantInfo = newVertexShaderConstantInfo;

            if (newPixelShaderDataAlignment)
            {
                pendingData.pixelShaderAlignment = newPixelShaderDataAlignment;
            }
            else
            {
                pendingData.pixelShaderAlignment = 0;

                for (const Shader::ConstantInfo& info : newPixelShaderConstantInfo)
                {
                    pendingData.pixelShaderAlignment += info.size;
                }
            }

            if (newVertexShaderDataAlignment)
            {
                pendingData.vertexShaderAlignment = newVertexShaderDataAlignment;
            }
            else
            {
                pendingData.vertexShaderAlignment = 0;

                for (const Shader::ConstantInfo& info : newVertexShaderConstantInfo)
                {
                    pendingData.vertexShaderAlignment += info.size;
                }
            }

            pendingData.pixelShaderFunction = newPixelShaderFunction;
            pendingData.vertexShaderFunction = newVertexShaderFunction;

            pendingData.dirty |= 0x01;

            return  true;
        }

        bool ShaderResource::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            data.dirty |= pendingData.dirty;
            pendingData.dirty = 0;

            if (data.dirty)
            {
                data.vertexAttributes = pendingData.vertexAttributes;
                data.pixelShaderAlignment = pendingData.pixelShaderAlignment;
                data.vertexShaderAlignment = pendingData.vertexShaderAlignment;

                data.pixelShaderData = std::move(pendingData.pixelShaderData);
                data.vertexShaderData = std::move(pendingData.vertexShaderData);
                data.pixelShaderConstantInfo = std::move(pendingData.pixelShaderConstantInfo);
                data.vertexShaderConstantInfo = std::move(pendingData.vertexShaderConstantInfo);
                data.pixelShaderFunction = std::move(pendingData.pixelShaderFunction);
                data.vertexShaderFunction = std::move(pendingData.vertexShaderFunction);
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
