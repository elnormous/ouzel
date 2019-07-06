// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP
#define OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <d3d11.h>
#include <set>
#include <vector>
#include "graphics/direct3d11/D3D11RenderResource.hpp"
#include "math/Color.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            class RenderDevice;
            class Texture;

            class RenderTarget final: public RenderResource
            {
            public:
                RenderTarget(RenderDevice& initRenderDevice,
                             const std::set<Texture*>& initColorTextures,
                             Texture* initDepthTexture);
                ~RenderTarget();

                void resolve();

                inline const std::vector<ID3D11RenderTargetView*>& getRenderTargetViews() const noexcept { return renderTargetViews; }
                inline auto getDepthStencilView() const noexcept { return depthStencilView; }

            private:
                std::set<Texture*> colorTextures;
                Texture* depthTexture = nullptr;

                std::vector<ID3D11RenderTargetView*> renderTargetViews;
                ID3D11DepthStencilView* depthStencilView = nullptr;
            };
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP
