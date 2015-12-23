// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Texture.h"

namespace ouzel
{
    class TextureD3D11: public Texture
    {
    public:
        TextureD3D11(Renderer* renderer);
        virtual ~TextureD3D11();
    };
}
