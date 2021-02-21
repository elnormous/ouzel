// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_RENDERER_MATERIAL_HPP
#define OUZEL_GRAPHICS_RENDERER_MATERIAL_HPP

#include "Renderer.hpp"

namespace ouzel::graphics::renderer
{
    class Material final
    {
    public:
        Material(Renderer& initRenderer):
            renderer{initRenderer},
            resource{initRenderer}
        {
        }

    private:
        Renderer& renderer;
        Renderer::Resource resource;
    };
}

#endif // OUZEL_GRAPHICS_RENDERER_MATERIAL_HPP
