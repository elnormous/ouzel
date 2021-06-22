// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_RENDERPASS_HPP
#define OUZEL_GRAPHICS_RENDERPASS_HPP

#include "RenderDevice.hpp"

namespace ouzel::graphics
{
    class Graphics;
    class Texture;

    class RenderPass final
    {
    public:
        RenderPass(Graphics& initGraphics);

    private:
        RenderDevice::Resource resource;
    };
}

#endif // OUZEL_GRAPHICS_RENDERPASS_HPP
