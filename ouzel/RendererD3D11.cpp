// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererD3D11.h"
#include "win32/Win32View.h"

namespace ouzel
{
    RendererD3D11::RendererD3D11(const Size2& size, bool fullscreen, Engine* engine):
        Renderer(size, fullscreen, engine, Driver::DIRECT3D11)
    {
        _view = new Win32View(size, this);
    }
    
    RendererD3D11::~RendererD3D11()
    {
        
    }

	bool RendererD3D11::activateTexture(Texture* texture, uint32_t layer)
	{
		return true;
	}
}
