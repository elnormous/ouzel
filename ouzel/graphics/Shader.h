// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "utils/Noncopyable.h"
#include "graphics/DataType.h"
#include "graphics/Vertex.h"

namespace ouzel
{
    namespace graphics
    {
        class ShaderResource;

        class Shader: public Noncopyable
        {
        public:
            struct ConstantInfo
            {
                ConstantInfo(std::string aName, DataType aDataType):
                    name(aName), dataType(aDataType), size(getDataTypeSize(aDataType)) {}

                std::string name;
                DataType dataType;
                uint32_t size;
            };

            Shader();
            virtual ~Shader();

            bool initFromBuffers(const std::vector<uint8_t>& newPixelShader,
                                 const std::vector<uint8_t>& newVertexShader,
                                 const std::vector<VertexAttribute>& newVertexAttributes,
                                 const std::vector<ConstantInfo>& newPixelShaderConstantInfo,
                                 const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                                 uint32_t newPixelShaderDataAlignment = 0,
                                 uint32_t newVertexShaderDataAlignment = 0,
                                 const std::string& pixelShaderFunction = "",
                                 const std::string& vertexShaderFunction = "");
            bool initFromFiles(const std::string& newPixelShader,
                               const std::string& newVertexShader,
                               const std::vector<VertexAttribute>& newVertexAttributes,
                               const std::vector<ConstantInfo>& newPixelShaderConstantInfo,
                               const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                               uint32_t newPixelShaderDataAlignment = 0,
                               uint32_t newVertexShaderDataAlignment = 0,
                               const std::string& newPixelShaderFunction = "",
                               const std::string& newVertexShaderFunction = "");

            ShaderResource* getResource() const { return resource; }

            const std::vector<VertexAttribute>& getVertexAttributes() const;
            uint32_t getVertexSize() const;

        private:
            ShaderResource* resource = nullptr;

            std::string pixelShaderFilename;
            std::string vertexShaderFilename;
        };
    } // namespace graphics
} // namespace ouzel
