// Ouzel by Elviss Strazdins

#include "RenderPass.hpp"
#include "Graphics.hpp"

namespace ouzel::graphics
{
    RenderPass::RenderPass(Graphics& initGraphics,
                           const std::vector<RenderTarget*>& initRenderTargets):
        resource{*initGraphics.getDevice()},
        renderTargets{initRenderTargets}
    {
        std::set<RenderDevice::ResourceId> renderTargetIds;

        for (const auto& renderTarget : renderTargets)
            renderTargetIds.insert(renderTarget ? renderTarget->getResource() : 0);

        initGraphics.addCommand(std::make_unique<InitRenderPassCommand>(resource,
                                                                        renderTargetIds));
    }
}
