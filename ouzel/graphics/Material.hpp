// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATERIAL_HPP
#define OUZEL_MATERIAL_HPP

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
            Material() = default;

            Material(const Material&) = delete;
            Material& operator=(const Material&) = delete;

            Material(Material&&) = delete;
            Material& operator=(Material&&) = delete;

            std::shared_ptr<BlendState> blendState;
            std::shared_ptr<Shader> shader;
            std::shared_ptr<Texture> textures[Texture::LAYERS];
            graphics::Renderer::CullMode cullMode = graphics::Renderer::CullMode::BACK;
            Color diffuseColor = Color::WHITE;
            float opacity = 1.0F;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_MATERIAL_HPP
