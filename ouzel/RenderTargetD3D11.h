// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <d3d11.h>
#include <memory>
#include "RenderTarget.h"

namespace ouzel
{
    class RendererD3D11;

    class RenderTargetD3D11: public RenderTarget
    {
        friend RendererD3D11;
    public:
        virtual ~RenderTargetD3D11();

        void clean();

        virtual bool init(Size2 const& size, bool depthBuffer);

        ID3D11RenderTargetView* getRenderTargetView() const { return _renderTargetView; }

    protected:
        RenderTargetD3D11();

        ID3D11RenderTargetView* _renderTargetView = nullptr;
    };
}
