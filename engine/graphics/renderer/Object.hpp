// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERER_OBJECT_HPP
#define OUZEL_GRAPHICS_RENDERER_OBJECT_HPP

#include "Renderer.hpp"
#include "../Buffer.hpp"
#include "../Material.hpp"
#include "../../math/Matrix.hpp"

namespace ouzel::graphics::renderer
{
    class Object final
    {
    public:
        Object(Renderer& initRenderer):
            renderer{initRenderer},
            resource{initRenderer}
        {
        }

    private:
        Renderer& renderer;
        Renderer::Resource resource;

        const Buffer* indexBuffer = nullptr;
        const Buffer* vertexBuffer = nullptr;
        const Material* material = nullptr;
        Matrix4F transform;
    };
}

#endif // OUZEL_GRAPHICS_RENDERER_OBJECT_HPP
