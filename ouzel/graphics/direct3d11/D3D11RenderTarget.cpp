// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "D3D11RenderTarget.hpp"
#include "D3D11RenderDevice.hpp"
#include "D3D11Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            RenderTarget::RenderTarget(RenderDevice& initRenderDevice,
                                       const std::set<Texture*>& initColorTextures,
                                       Texture* initDepthTexture):
                RenderResource(initRenderDevice),
                colorTextures(initColorTextures),
                depthTexture(initDepthTexture)
            {
                for (const Texture* colorTexture : colorTextures)
                    if (colorTexture)
                        renderTargetViews.push_back(colorTexture->getRenderTargetView());

                depthStencilView = depthTexture ? depthTexture->getDepthStencilView() : nullptr;
            }

            RenderTarget::~RenderTarget()
            {
            }

            void RenderTarget::resolve()
            {
                for (Texture* colorTexture : colorTextures)
                    colorTexture->resolve();

                if (depthTexture)
                    depthTexture->resolve();
            }
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif
