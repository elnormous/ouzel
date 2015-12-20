//
//  RenderTexture.hpp
//  Ouzel
//
//  Created by Elviss Strazdins on 18/12/15.
//  Copyright © 2015 Bool Games. All rights reserved.
//

#pragma once

#include "Noncopyable.h"

namespace ouzel
{
    class RenderTexture: public Noncopyable
    {
    public:
        RenderTexture();
        virtual ~RenderTexture();
    };
}
