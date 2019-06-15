// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_SHADER_HPP
#define OUZEL_GRAPHICS_SHADER_HPP

#include <set>
#include <string>
#include <vector>
#include "graphics/GraphicsResource.hpp"
#include "graphics/DataType.hpp"
#include "graphics/Vertex.hpp"
#include "utils/Inline.h"

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
                    name(initName), dataType(initDataType) {}

                std::string name;
                DataType dataType;
            };

            Shader() = default;

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

            ALWAYSINLINE const Resource& getResource() const { return resource; }

            const std::set<Vertex::Attribute::Usage>& getVertexAttributes() const;

        private:
            Resource resource;

            std::set<Vertex::Attribute::Usage> vertexAttributes;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_SHADER_HPP
