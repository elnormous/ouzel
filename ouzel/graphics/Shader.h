// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Shader: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            struct ConstantInfo
            {
                std::string name;
                uint32_t size;
            };

            virtual ~Shader();
            virtual void free() override;

            virtual bool initFromFiles(const std::string& newPixelShader,
                                       const std::string& newVertexShader,
                                       uint32_t newVertexAttributes,
                                       const std::vector<ConstantInfo>& newPixelShaderConstantInfo,
                                       const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                                       uint32_t newPixelShaderDataAlignment = 0,
                                       uint32_t newVertexShaderDataAlignment = 0,
                                       const std::string& newPixelShaderFunction = "",
                                       const std::string& newVertexShaderFunction = "");
            virtual bool initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                         const std::vector<uint8_t>& newVertexShader,
                                         uint32_t newVertexAttributes,
                                         const std::vector<ConstantInfo>& newPixelShaderConstantInfo,
                                         const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                                         uint32_t newPixelShaderDataAlignment = 0,
                                         uint32_t newVertexShaderDataAlignment = 0,
                                         const std::string& pixelShaderFunction = "",
                                         const std::string& vertexShaderFunction = "");

            uint32_t getVertexAttributes() const { return data.vertexAttributes; }

            bool isReady() const { return ready; }

        protected:
            Shader();
            virtual void update() override;

            std::string pixelShaderFilename;
            std::string vertexShaderFilename;

            struct Data
            {
                uint32_t vertexAttributes = 0;

                std::vector<uint8_t> pixelShaderData;
                std::vector<uint8_t> vertexShaderData;
                std::string pixelShaderFunction;
                std::string vertexShaderFunction;

                std::vector<ConstantInfo> pixelShaderConstantInfo;
                uint32_t pixelShaderAlignment = 0;
                std::vector<ConstantInfo> vertexShaderConstantInfo;
                uint32_t vertexShaderAlignment = 0;
            };

            Data data;
            Data uploadData;

            bool ready = false;
            bool dirty = false;
        };
    } // namespace graphics
} // namespace ouzel
