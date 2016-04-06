// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <d3d11.h>
#include <memory>
#include "RenderTarget.h"

namespace ouzel
{
    namespace video
    {
        class RendererD3D11;

        class RenderTargetD3D11: public RenderTarget
        {
            friend RendererD3D11;
        public:
            virtual ~RenderTargetD3D11();

            virtual bool init(const Size2& size, bool depthBuffer);

            ID3D11RenderTargetView* getRenderTargetView() const { return _renderTargetView; }

        protected:
            RenderTargetD3D11();
            
            void destroy();

            ID3D11RenderTargetView* _renderTargetView = nullptr;
        };
    } // namespace video
} // namespace ouzel
