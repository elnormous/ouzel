// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERER_RENDERER_HPP
#define OUZEL_GRAPHICS_RENDERER_RENDERER_HPP

#include <cstddef>
#include "Camera.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Object.hpp"
#include "../RenderDevice.hpp"

namespace ouzel::graphics::renderer
{
    class Renderer final
    {
    public:
        Renderer() = default;

    private:
        void render();

        enum class RenderStage
        {
            depth,
            color,
            postProcess
        };

        std::size_t lastResourceId = 0;
        std::set<std::size_t> deletedResourceIds;
    };
}

#endif // OUZEL_GRAPHICS_RENDERER_RENDERER_HPP
