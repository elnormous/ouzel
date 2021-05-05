// Ouzel by Elviss Strazdins

#include "RenderTarget.hpp"
#include "Graphics.hpp"
#include "Texture.hpp"

namespace ouzel::graphics
{
    RenderTarget::RenderTarget(Graphics& initGraphics,
                               const std::vector<Texture*>& initColorTextures,
                               Texture* initDepthTexture):
        resource{*initGraphics.getDevice()},
        colorTextures{initColorTextures},
        depthTexture{initDepthTexture}
    {
        std::set<std::size_t> colorTextureIds;

        for (const auto& colorTexture : colorTextures)
            colorTextureIds.insert(colorTexture ? colorTexture->getResource() : 0);

        initGraphics.addCommand(std::make_unique<InitRenderTargetCommand>(resource,
                                                                          colorTextureIds,
                                                                          depthTexture ? depthTexture->getResource() : RenderDevice::ResourceId(0)));
    }
}
