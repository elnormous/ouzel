// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ShaderResource.hpp"

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

        bool ShaderResource::init(const std::vector<uint8_t>& newPixelShader,
                                  const std::vector<uint8_t>& newVertexShader,
                                  const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                                  const std::vector<Shader::ConstantInfo>& newPixelShaderConstantInfo,
                                  const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                  uint32_t newPixelShaderDataAlignment,
                                  uint32_t newVertexShaderDataAlignment,
                                  const std::string& newPixelShaderFunction,
                                  const std::string& newVertexShaderFunction)
        {
            pixelShaderData = newPixelShader;
            vertexShaderData = newVertexShader;
            vertexAttributes = newVertexAttributes;
            pixelShaderConstantInfo = newPixelShaderConstantInfo;
            vertexShaderConstantInfo = newVertexShaderConstantInfo;

            if (newPixelShaderDataAlignment)
            {
                pixelShaderAlignment = newPixelShaderDataAlignment;
            }
            else
            {
                pixelShaderAlignment = 0;

                for (const Shader::ConstantInfo& info : newPixelShaderConstantInfo)
                {
                    pixelShaderAlignment += info.size;
                }
            }

            if (newVertexShaderDataAlignment)
            {
                vertexShaderAlignment = newVertexShaderDataAlignment;
            }
            else
            {
                vertexShaderAlignment = 0;

                for (const Shader::ConstantInfo& info : newVertexShaderConstantInfo)
                {
                    vertexShaderAlignment += info.size;
                }
            }

            pixelShaderFunction = newPixelShaderFunction;
            vertexShaderFunction = newVertexShaderFunction;

            return  true;
        }
    } // namespace graphics
} // namespace ouzel
