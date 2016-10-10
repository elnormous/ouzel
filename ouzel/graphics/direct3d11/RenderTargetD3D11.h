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
        class RenderTargetD3D11: public RenderTarget
        {
        public:
            RenderTargetD3D11();
            virtual ~RenderTargetD3D11();
            virtual void free() override;

            ID3D11RenderTargetView* getRenderTargetView() const { return renderTargetView; }
            const D3D11_VIEWPORT& getViewport() const { return viewport; }

            const float* getFrameBufferClearColor() const { return frameBufferClearColor; }

        protected:
            virtual bool upload() override;

            ID3D11RenderTargetView* renderTargetView = nullptr;
            D3D11_VIEWPORT viewport;

            FLOAT frameBufferClearColor[4];
        };
    } // namespace graphics
} // namespace ouzel
