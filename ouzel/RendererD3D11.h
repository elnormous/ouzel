// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Renderer.h"

namespace ouzel
{
    class RendererD3D11: public Renderer
    {
    public:
        RendererD3D11(const Size2& size, bool fullscreen, Engine* engine);
		virtual ~RendererD3D11();

        virtual bool activateTexture(Texture* texture, uint32_t layer) override;
    };
}
