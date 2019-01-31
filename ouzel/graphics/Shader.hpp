// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_SHADER_HPP
#define OUZEL_GRAPHICS_SHADER_HPP

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

        class Shader final
        {
        public:
            class ConstantInfo final
            {
            public:
                ConstantInfo(const std::string& initName, DataType initDataType):
                    name(initName), dataType(initDataType), size(getDataTypeSize(initDataType)) {}

                std::string name;
                DataType dataType;
                uint32_t size;
            };

            explicit Shader(Renderer& initRenderer);
            Shader(Renderer& initRenderer,
                   const std::vector<uint8_t>& initFragmentShader,
                   const std::vector<uint8_t>& initVertexShader,
                   const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                   const std::vector<ConstantInfo>& initFragmentShaderConstantInfo,
                   const std::vector<ConstantInfo>& initVertexShaderConstantInfo,
                   uint32_t initFragmentShaderDataAlignment = 0,
                   uint32_t initVertexShaderDataAlignment = 0,
                   const std::string& fragmentShaderFunction = "",
                   const std::string& vertexShaderFunction = "");
            ~Shader();

            Shader(const Shader&) = delete;
            Shader& operator=(const Shader&) = delete;

            Shader(Shader&& other):
                renderer(other.renderer)
            {
                resource = other.resource;
                vertexAttributes = std::move(other.vertexAttributes);

                other.resource = 0;
            }

            Shader& operator=(Shader&& other)
            {
                if (&other != this)
                {
                    resource = other.resource;
                    vertexAttributes = std::move(other.vertexAttributes);

                    other.resource = 0;
                }

                return *this;
            }

            void init(const std::vector<uint8_t>& newFragmentShader,
                      const std::vector<uint8_t>& newVertexShader,
                      const std::set<Vertex::Attribute::Usage>& newVertexAttributes,
                      const std::vector<ConstantInfo>& newFragmentShaderConstantInfo,
                      const std::vector<ConstantInfo>& newVertexShaderConstantInfo,
                      uint32_t newFragmentShaderDataAlignment = 0,
                      uint32_t newVertexShaderDataAlignment = 0,
                      const std::string& fragmentShaderFunction = "",
                      const std::string& vertexShaderFunction = "");

            inline uintptr_t getResource() const { return resource; }

            const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const;

        private:
            Renderer& renderer;
            uintptr_t resource = 0;

            std::set<Vertex::Attribute::Usage> vertexAttributes;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_SHADER_HPP
