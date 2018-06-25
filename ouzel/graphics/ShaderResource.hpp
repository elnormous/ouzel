// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "graphics/RenderResource.hpp"
#include "graphics/Shader.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"
#include "math/Matrix4.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class ShaderResource: public RenderResource
        {
            friend Renderer;
        public:
            virtual ~ShaderResource();

            virtual void init(const std::vector<uint8_t>& newFragmentShader,
                              const std::vector<uint8_t>& newVertexShader,
                              const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                              const std::vector<Shader::ConstantInfo>& newFragmentShaderConstantInfo,
                              const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                              uint32_t newFragmentShaderDataAlignment = 0,
                              uint32_t newVertexShaderDataAlignment = 0,
                              const std::string& newFragmentShaderFunction = "",
                              const std::string& newVertexShaderFunction = "");

            inline const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const { return vertexAttributes; }

            inline uint32_t getFragmentShaderAlignment() const { return fragmentShaderAlignment; }
            inline uint32_t getVertexShaderAlignment() const { return vertexShaderAlignment; }

        protected:
            ShaderResource();

            std::set<Vertex::Attribute::Usage> vertexAttributes;

            std::vector<uint8_t> fragmentShaderData;
            std::vector<uint8_t> vertexShaderData;
            std::string fragmentShaderFunction;
            std::string vertexShaderFunction;

            std::vector<Shader::ConstantInfo> fragmentShaderConstantInfo;
            uint32_t fragmentShaderAlignment = 0;
            std::vector<Shader::ConstantInfo> vertexShaderConstantInfo;
            uint32_t vertexShaderAlignment = 0;
        };
    } // namespace graphics
} // namespace ouzel
