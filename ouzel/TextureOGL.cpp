//
//  RenderTexture.hpp
//  Ouzel
//
//  Created by Elviss Strazdins on 19/12/15.
//  Copyright © 2015 Bool Games. All rights reserved.
//

#include "TextureOGL.h"

namespace ouzel
{
    TextureOGL::TextureOGL(const std::string& filename, Renderer* renderer):
        Texture(filename, renderer)
    {
        
    }
    
    TextureOGL::~TextureOGL()
    {
        
    }
}
