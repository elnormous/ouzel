// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include <atomic>
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

            virtual void setClearColor(Color color) override;

            ID3D11RenderTargetView* getRenderTargetView() const { return renderTargetView; }
            const D3D11_VIEWPORT& getViewport() const { return viewport; }

            const float* getFrameBufferClearColor() const { return frameBufferClearColor; }

        protected:
            RenderTargetD3D11();
            virtual bool upload() override;

            ID3D11RenderTargetView* renderTargetView = nullptr;
            D3D11_VIEWPORT viewport;

            FLOAT frameBufferClearColor[4];

            std::atomic<bool> dirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
