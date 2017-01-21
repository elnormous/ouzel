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
            struct ConstantInfo
            {
                ConstantInfo(std::string aName, Shader::DataType aDataType):
                    name(aName), dataType(aDataType)
                {
                    switch (aDataType)
                    {
                        case Shader::DataType::FLOAT:
                            size = sizeof(float);
                            break;
                        case Shader::DataType::FLOAT_VECTOR2:
                            size = 2 * sizeof(float);
                            break;
                        case Shader::DataType::FLOAT_VECTOR3:
                            size = 3 * sizeof(float);
                            break;
                        case Shader::DataType::FLOAT_VECTOR4:
                            size = 4 * sizeof(float);
                            break;
                        case Shader::DataType::FLOAT_MATRIX3:
                            size = 9 * sizeof(float);
                            break;
                        case Shader::DataType::FLOAT_MATRIX4:
                            size = 16 * sizeof(float);
                            break;
                    }
                }

                std::string name;
                Shader::DataType dataType;
                uint32_t size;
            };

            virtual ~ShaderResource();
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

        protected:
            ShaderResource();
            virtual void update() override;
            virtual bool upload() override;

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

            Data uploadData;
            std::mutex uploadMutex;

        private:
            Data data;

            std::string pixelShaderFilename;
            std::string vertexShaderFilename;

            Data currentData;
        };
    } // namespace graphics
} // namespace ouzel
