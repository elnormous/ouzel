// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_RENDERPASS_HPP
#define OUZEL_GRAPHICS_RENDERPASS_HPP

#include "RenderDevice.hpp"
#include "RenderTarget.hpp"

namespace ouzel::graphics
{
    class Graphics;
    class Texture;

    class RenderPass final
    {
    public:
        RenderPass(Graphics& initGraphics,
                   const std::vector<RenderTarget*>& initRenderTargets);

        auto& getResource() const noexcept { return resource; }

    private:
        RenderDevice::Resource resource;
        std::vector<RenderTarget*> renderTargets;
    };
}

#endif // OUZEL_GRAPHICS_RENDERPASS_HPP
