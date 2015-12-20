//
//  RenderTexture.hpp
//  Ouzel
//
//  Created by Elviss Strazdins on 19/12/15.
//  Copyright © 2015 Bool Games. All rights reserved.
//

#pragma once

#include "Texture.h"

namespace ouzel
{
    class TextureOGL: public Texture
    {
    public:
        TextureOGL(const std::string& filename, Renderer* renderer);
        virtual ~TextureOGL();
    };
}
