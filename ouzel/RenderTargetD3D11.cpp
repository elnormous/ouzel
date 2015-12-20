// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetD3D11.h"

namespace ouzel
{
    RenderTargetD3D11::RenderTargetD3D11(const std::string& filename, Renderer* renderer):
        Texture(filename, renderer)
    {
        
    }
    
    RenderTargetD3D11::~RenderTargetD3D11()
    {
        
    }
}
