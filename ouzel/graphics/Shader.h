// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class ShaderResource;

        class Shader: public Noncopyable
        {
        public:
            enum class DataType
            {
                FLOAT,
                FLOAT_VECTOR2,
                FLOAT_VECTOR3,
                FLOAT_VECTOR4,
                FLOAT_MATRIX3,
                FLOAT_MATRIX4
            };

            struct ConstantInfo
            {
                ConstantInfo(std::string aName, Shader::DataType aDataType):
                name(aName), dataType(aDataType)
                {
                    switch (aDataType)
                    {
                        case DataType::FLOAT:
                            size = sizeof(float);
                            break;
                        case DataType::FLOAT_VECTOR2:
                            size = 2 * sizeof(float);
                            break;
                        case DataType::FLOAT_VECTOR3:
                            size = 3 * sizeof(float);
                            break;
                        case DataType::FLOAT_VECTOR4:
                            size = 4 * sizeof(float);
                            break;
                        case DataType::FLOAT_MATRIX3:
                            size = 9 * sizeof(float);
                            break;
                        case DataType::FLOAT_MATRIX4:
                            size = 16 * sizeof(float);
                            break;
                    }
                }

                std::string name;
                Shader::DataType dataType;
                uint32_t size;
            };
            
            Shader();
            virtual ~Shader();

            bool initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                 const std::vector<uint8_t>& newVertexShader,
                                 uint32_t newVertexAttributes,
                                 const std::vector<ConstantInfo>& newPixelShaderConstantInfo,
                                 const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                                 uint32_t newPixelShaderDataAlignment = 0,
                                 uint32_t newVertexShaderDataAlignment = 0,
                                 const std::string& pixelShaderFunction = "",
                                 const std::string& vertexShaderFunction = "");
            bool initFromFiles(const std::string& newPixelShader,
                               const std::string& newVertexShader,
                               uint32_t newVertexAttributes,
                               const std::vector<ConstantInfo>& newPixelShaderConstantInfo,
                               const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                               uint32_t newPixelShaderDataAlignment = 0,
                               uint32_t newVertexShaderDataAlignment = 0,
                               const std::string& newPixelShaderFunction = "",
                               const std::string& newVertexShaderFunction = "");

            ShaderResource* getResource() const { return resource; }

            uint32_t getVertexAttributes() const { return vertexAttributes; }

        private:
            ShaderResource* resource = nullptr;

            std::string pixelShaderFilename;
            std::string vertexShaderFilename;

            uint32_t vertexAttributes = 0;
        };
    } // namespace graphics
} // namespace ouzel
