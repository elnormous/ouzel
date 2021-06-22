// Ouzel by Elviss Strazdins

#include "RenderPass.hpp"
#include "Graphics.hpp"

namespace ouzel::graphics
{
    RenderPass::RenderPass(Graphics& initGraphics):
        resource{*initGraphics.getDevice()}
    {
        initGraphics.addCommand(std::make_unique<InitRenderPassCommand>(resource));
    }
}
