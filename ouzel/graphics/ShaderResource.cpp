// Copyright (C) 2018 Elviss Strazdins
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

        bool ShaderResource::init(const std::vector<uint8_t>& newFragmentShader,
                                  const std::vector<uint8_t>& newVertexShader,
                                  const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                                  const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                                  const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                  uint32_t newFragmentShaderDataAlignment,
                                  uint32_t newVertexShaderDataAlignment,
                                  const std::string& newFragmentShaderFunction,
                                  const std::string& newVertexShaderFunction)
        {
            fragmentShaderData = newFragmentShader;
            vertexShaderData = newVertexShader;
            vertexAttributes = newVertexAttributes;
            fragmentShaderConstantInfo = newFragmentShaderConstantInfo;
            vertexShaderConstantInfo = newVertexShaderConstantInfo;

            if (newFragmentShaderDataAlignment)
            {
                fragmentShaderAlignment = newFragmentShaderDataAlignment;
            }
            else
            {
                fragmentShaderAlignment = 0;

                for (const Shader::ConstantInfo& info : newFragmentShaderConstantInfo)
                    fragmentShaderAlignment += info.size;
            }

            if (newVertexShaderDataAlignment)
            {
                vertexShaderAlignment = newVertexShaderDataAlignment;
            }
            else
            {
                vertexShaderAlignment = 0;

                for (const Shader::ConstantInfo& info : newVertexShaderConstantInfo)
                    vertexShaderAlignment += info.size;
            }

            fragmentShaderFunction = newFragmentShaderFunction;
            vertexShaderFunction = newVertexShaderFunction;

            return  true;
        }
    } // namespace graphics
} // namespace ouzel
