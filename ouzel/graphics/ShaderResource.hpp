// Copyright (C) 2018 Elviss Strazdins
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

            virtual bool init(const std::vector<uint8_t>& newPixelShader,
                              const std::vector<uint8_t>& newVertexShader,
                              const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                              const std::vector<Shader::ConstantInfo>& newPixelShaderConstantInfo,
                              const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                              uint32_t newPixelShaderDataAlignment = 0,
                              uint32_t newVertexShaderDataAlignment = 0,
                              const std::string& newPixelShaderFunction = "",
                              const std::string& newVertexShaderFunction = "");

            const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const { return vertexAttributes; }

            uint32_t getPixelShaderAlignment() const { return pixelShaderAlignment; }
            uint32_t getVertexShaderAlignment() const { return vertexShaderAlignment; }

        protected:
            ShaderResource();

            std::set<Vertex::Attribute::Usage> vertexAttributes;

            std::vector<uint8_t> pixelShaderData;
            std::vector<uint8_t> vertexShaderData;
            std::string pixelShaderFunction;
            std::string vertexShaderFunction;

            std::vector<Shader::ConstantInfo> pixelShaderConstantInfo;
            uint32_t pixelShaderAlignment = 0;
            std::vector<Shader::ConstantInfo> vertexShaderConstantInfo;
            uint32_t vertexShaderAlignment = 0;
        };
    } // namespace graphics
} // namespace ouzel
