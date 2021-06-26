// Ouzel by Elviss Strazdins

#include "RenderPass.hpp"
#include "Graphics.hpp"

namespace ouzel::graphics
{
    RenderPass::RenderPass(Graphics& initGraphics,
                           const std::vector<RenderTarget*>& initRenderTargets):
        graphics{&initGraphics},
        resource{*initGraphics.getDevice()},
        renderTargets{initRenderTargets}
    {
        std::set<RenderDevice::ResourceId> renderTargetIds;

        for (const auto& renderTarget : renderTargets)
            renderTargetIds.insert(renderTarget ? renderTarget->getResource() : 0);

        graphics->addCommand(std::make_unique<InitRenderPassCommand>(resource,
                                                                        renderTargetIds));
    }

    void RenderPass::setRenderTargets(const std::vector<RenderTarget*>& newRenderTargets)
    {
        std::set<RenderDevice::ResourceId> renderTargetIds;

        for (const auto& renderTarget : renderTargets)
            renderTargetIds.insert(renderTarget ? renderTarget->getResource() : 0);

        if (resource)
            graphics->addCommand(std::make_unique<SetRenderPassParametersCommand>(resource,
                                                                                  renderTargetIds));
    }
}
