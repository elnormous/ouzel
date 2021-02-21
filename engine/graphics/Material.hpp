// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_MATERIAL_HPP
#define OUZEL_GRAPHICS_MATERIAL_HPP

#include <memory>
#include "Graphics.hpp"
#include "BlendState.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "../math/Color.hpp"

namespace ouzel::graphics
{
    class Material final
    {
    public:
        static constexpr std::uint32_t textureLayers = 4U;

        Material() = default;

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;

        Material(Material&&) = delete;
        Material& operator=(Material&&) = delete;

        const BlendState* blendState = nullptr;
        const Shader* shader = nullptr;
        std::shared_ptr<Texture> textures[textureLayers];
        CullMode cullMode = CullMode::back;
        Color diffuseColor = Color::white();
        float opacity = 1.0F;
    };
}

#endif // OUZEL_GRAPHICS_MATERIAL_HPP
