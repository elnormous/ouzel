// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
        class ShaderResource: public RenderResource
        {
        public:
            virtual ~ShaderResource();

            inline const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const { return vertexAttributes; }

            inline uint32_t getFragmentShaderAlignment() const { return fragmentShaderAlignment; }
            inline uint32_t getVertexShaderAlignment() const { return vertexShaderAlignment; }

        protected:
            ShaderResource(RenderDevice& initRenderDevice);

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
