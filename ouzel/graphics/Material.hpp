// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "Renderer.hpp"
#include "Texture.hpp"
#include "math/Color.hpp"

namespace ouzel
{
    namespace graphics
    {
        class BlendState;
        class Shader;
        class Texture;

        class Material final
        {
        public:
            Material() = default;
            ~Material() = default;

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
