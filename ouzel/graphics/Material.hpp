// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_MATERIAL_HPP
#define OUZEL_GRAPHICS_MATERIAL_HPP

#include <memory>
#include "graphics/Renderer.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "math/Color.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Material final
        {
        public:
            static constexpr uint32_t TEXTURE_LAYERS = 4;

            Material() = default;

            Material(const Material&) = delete;
            Material& operator=(const Material&) = delete;

            Material(Material&&) = delete;
            Material& operator=(Material&&) = delete;

            BlendState* blendState = nullptr;
            Shader* shader = nullptr;
            Texture* textures[TEXTURE_LAYERS]{nullptr};
            graphics::CullMode cullMode = graphics::CullMode::BACK;
            Color diffuseColor = Color::WHITE;
            float opacity = 1.0F;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_MATERIAL_HPP
