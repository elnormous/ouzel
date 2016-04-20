// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendState.h"

namespace ouzel
{
    namespace graphics
    {
        BlendState::BlendState()
        {

        }

        bool BlendState::init(bool enableBlending,
                              BlendFactor colorBlendSource, BlendFactor colorBlendDest,
                              BlendOperation colorOperation,
                              BlendFactor alphaBlendSource, BlendFactor alphaBlendDest,
                              BlendOperation alphaOperation)
        {
            _enableBlending = enableBlending;
            _colorBlendSource = colorBlendSource;
            _colorBlendDest = colorBlendDest;
            _colorOperation = colorOperation;
            _alphaBlendSource = alphaBlendSource;
            _alphaBlendDest = alphaBlendDest;
            _alphaOperation = alphaOperation;

            return true;
        }

    } // namespace graphics
} // namespace ouzel
