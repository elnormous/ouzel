// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "graphics/Shader.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class ShaderResource: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~ShaderResource();

            bool initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                 const std::vector<uint8_t>& newVertexShader,
                                 const std::vector<VertexAttribute>& newVertexAttributes,
                                 const std::vector<Shader::ConstantInfo>& newPixelShaderConstantInfo,
                                 const std::vector<Shader::ConstantInfo>& newVertexShaderConstantInfo,
                                 uint32_t newPixelShaderDataAlignment = 0,
                                 uint32_t newVertexShaderDataAlignment = 0,
                                 const std::string& pixelShaderFunction = "",
                                 const std::string& vertexShaderFunction = "");

            const std::vector<VertexAttribute>& getVertexAttributes() const { return data.vertexAttributes; }

        protected:
            ShaderResource();
            virtual bool upload() override;

            struct Data
            {
                uint32_t dirty = 0;
                
                std::vector<VertexAttribute> vertexAttributes;

                std::vector<uint8_t> pixelShaderData;
                std::vector<uint8_t> vertexShaderData;
                std::string pixelShaderFunction;
                std::string vertexShaderFunction;

                std::vector<Shader::ConstantInfo> pixelShaderConstantInfo;
                uint32_t pixelShaderAlignment = 0;
                std::vector<Shader::ConstantInfo> vertexShaderConstantInfo;
                uint32_t vertexShaderAlignment = 0;
            };

            Data data;

        private:
            Data pendingData;
            std::mutex uploadMutex;
        };
    } // namespace graphics
} // namespace ouzel
