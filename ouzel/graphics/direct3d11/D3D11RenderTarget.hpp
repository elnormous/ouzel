// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
        class D3D11RenderDevice;
        class D3D11Texture;

        class D3D11RenderTarget final: public D3D11RenderResource
        {
        public:
            explicit D3D11RenderTarget(D3D11RenderDevice& renderDeviceD3D11,
                                       const std::set<D3D11Texture*>& initColorTextures,
                                       D3D11Texture* initDepthTexture);
            ~D3D11RenderTarget();

            void resolve();

            const std::vector<ID3D11RenderTargetView*>& getRenderTargetViews() const { return renderTargetViews; }
            ID3D11DepthStencilView* getDepthStencilView() const { return depthStencilView; }

        private:
            std::set<D3D11Texture*> colorTextures;
            D3D11Texture* depthTexture = nullptr;

            std::vector<ID3D11RenderTargetView*> renderTargetViews;
            ID3D11DepthStencilView* depthStencilView = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP
