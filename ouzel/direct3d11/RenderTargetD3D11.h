// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <d3d11.h>
#include <memory>
#include "RenderTarget.h"

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

            virtual bool init(const Size2& newSize, bool depthBuffer);

            ID3D11RenderTargetView* getRenderTargetView() const { return renderTargetView; }

        protected:
            RenderTargetD3D11();

            void destroy();

            ID3D11RenderTargetView* renderTargetView = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
