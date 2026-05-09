#ifndef FLUXION_GRAPHICS_RENDERPASS_HPP
#define FLUXION_GRAPHICS_RENDERPASS_HPP

#include "RenderDevice.hpp"
#include "RenderTarget.hpp"

namespace fluxion::graphics
{
    class Graphics;
    class Texture;

    class RenderPass final
    {
    public:
        RenderPass() = default;

        RenderPass(Graphics& initGraphics,
                   const std::vector<RenderTarget*>& initRenderTargets);

        auto& getResource() const noexcept { return resource; }

        void setRenderTargets(const std::vector<RenderTarget*>& newRenderTargets);

    private:
        Graphics* graphics = nullptr;
        RenderDevice::Resource resource;
        std::vector<RenderTarget*> renderTargets;
    };
}

#endif // FLUXION_GRAPHICS_RENDERPASS_HPP
