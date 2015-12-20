// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererD3D11.h"

namespace ouzel
{
    RendererD3D11::RendererD3D11(const std::string& filename, Renderer* renderer):
        Texture(filename, renderer)
    {
        
    }
    
    RendererD3D11::~RendererD3D11()
    {
        
    }
}
