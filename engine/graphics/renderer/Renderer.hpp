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
        struct Command final
        {
            enum class Type
            {
                initCamera,
                initLight,
                initMaterial,
                initObject
            };

            Type type;
        };

        Renderer() = default;

        std::size_t createResource() { return 0; }
        void destroyResource(std::size_t id) { (void)id; }

    private:
        void render();

        std::size_t lastResourceId = 0;
        std::set<std::size_t> deletedResourceIds;
    };
}

#endif // OUZEL_GRAPHICS_RENDERER_RENDERER_HPP
