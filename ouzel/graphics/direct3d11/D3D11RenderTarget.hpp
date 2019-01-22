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
                                       bool initClearColorBuffer = true,
                                       bool initClearDepthBuffer = false,
                                       Color initClearColor = Color(),
                                       float initClearDepth = 1.0F);
            ~D3D11RenderTarget();

            void addColorTexture(D3D11Texture* texture);
            void removeColorTexture(D3D11Texture* texture);
            void setDepthTexture(D3D11Texture* texture);

            void setClearColorBuffer(bool clear);
            void setClearDepthBuffer(bool clear);
            void setClearColor(Color color);
            void setClearDepth(float newClearDepth);
            void resolve();

            const std::vector<ID3D11RenderTargetView*>& getRenderTargetViews() const { return renderTargetViews; }
            ID3D11DepthStencilView* getDepthStencilView() const { return depthStencilView; }

            inline bool getClearFrameBufferView() const { return clearFrameBufferView; }
            inline bool getClearDepthBufferView() const { return clearDepthBufferView; }
            inline const float* getFrameBufferClearColor() const { return frameBufferClearColor; }
            inline float getClearDepth() const { return clearDepth; }

        private:
            std::set<D3D11Texture*> colorTextures;
            D3D11Texture* depthTexture = nullptr;

            std::vector<ID3D11RenderTargetView*> renderTargetViews;
            ID3D11DepthStencilView* depthStencilView = nullptr;

            bool clearFrameBufferView = true;
            bool clearDepthBufferView = false;
            FLOAT frameBufferClearColor[4]{0.0F, 0.0F, 0.0F, 0.0F};
            float clearDepth = 1.0F;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11RENDERTARGET_HPP
