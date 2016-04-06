// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateMetal.h"

namespace ouzel
{
    namespace video
    {
        BlendStateMetal::BlendStateMetal()
        {

        }

        BlendStateMetal::~BlendStateMetal()
        {
            destroy();
        }

        void BlendStateMetal::destroy()
        {
            if (_blendState) [_blendState release];
        }
    } // namespace video
} // namespace ouzel
