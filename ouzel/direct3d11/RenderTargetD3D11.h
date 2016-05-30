// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include <memory>
#include "graphics/RenderTarget.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11;

        class RenderTargetD3D11: public RenderTarget
        {
            friend RendererD3D11;
        public:
            virtual ~RenderTargetD3D11();
            virtual void free() override;

            virtual bool init(const Size2& newSize, bool depthBuffer);

            ID3D11RenderTargetView* getRenderTargetView() const { return renderTargetView; }
            const D3D11_VIEWPORT& getViewport() const { return viewport; }

        protected:
            RenderTargetD3D11();

            ID3D11RenderTargetView* renderTargetView = nullptr;
            D3D11_VIEWPORT viewport;
        };
    } // namespace graphics
} // namespace ouzel
