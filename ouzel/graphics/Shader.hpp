// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <vector>
#include "graphics/DataType.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class ShaderResource;

        class Shader final
        {
        public:
            class ConstantInfo
            {
            public:
                ConstantInfo(const std::string& initName, DataType initDataType):
                    name(initName), dataType(initDataType), size(getDataTypeSize(initDataType)) {}

                std::string name;
                DataType dataType;
                uint32_t size;
            };

            Shader(Renderer* initRenderer);
            ~Shader();

            Shader(const Shader&) = delete;
            Shader& operator=(const Shader&) = delete;

            Shader(Shader&&) = delete;
            Shader& operator=(Shader&&) = delete;

            void init(const std::vector<uint8_t>& newFragmentShader,
                      const std::vector<uint8_t>& newVertexShader,
                      const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                      const std::vector<ConstantInfo>& newFragmentShaderConstantInfo,
                      const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                      uint32_t newFragmentShaderDataAlignment = 0,
                      uint32_t newVertexShaderDataAlignment = 0,
                      const std::string& fragmentShaderFunction = "",
                      const std::string& vertexShaderFunction = "");
            void init(const std::string& newFragmentShader,
                      const std::string& newVertexShader,
                      const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                      const std::vector<ConstantInfo>& newFragmentShaderConstantInfo,
                      const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                      uint32_t newFragmentShaderDataAlignment = 0,
                      uint32_t newVertexShaderDataAlignment = 0,
                      const std::string& newFragmentShaderFunction = "",
                      const std::string& newVertexShaderFunction = "");

            inline ShaderResource* getResource() const { return resource; }

            const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const;

        private:
            Renderer* renderer;
            ShaderResource* resource = nullptr;

            std::set<Vertex::Attribute::Usage> vertexAttributes;

            std::string fragmentShaderFilename;
            std::string vertexShaderFilename;
        };
    } // namespace graphics
} // namespace ouzel
