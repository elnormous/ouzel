// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_RENDERER_LIGHT_HPP
#define OUZEL_GRAPHICS_RENDERER_LIGHT_HPP

#include "Renderer.hpp"
#include "../../math/Matrix.hpp"

namespace ouzel::graphics::renderer
{
    class Light final
    {
    public:
        enum class Type
        {
            point,
            spot,
            directional
        };

        Light(Renderer& initRenderer):
            renderer{initRenderer},
            resource{initRenderer}
        {
        }

    private:
        Renderer& renderer;
        Renderer::Resource resource;
        Type type = Type::point;
        math::Matrix<float, 4> transform;
    };
}

#endif // OUZEL_GRAPHICS_RENDERER_LIGHT_HPP
