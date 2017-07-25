// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateInterface.h"

namespace ouzel
{
    namespace graphics
    {
        BlendStateInterface::BlendStateInterface()
        {
        }

        BlendStateInterface::~BlendStateInterface()
        {
        }

        bool BlendStateInterface::init(bool newEnableBlending,
                                       BlendState::BlendFactor newColorBlendSource, BlendState::BlendFactor newColorBlendDest,
                                       BlendState::BlendOperation newColorOperation,
                                       BlendState::BlendFactor newAlphaBlendSource, BlendState::BlendFactor newAlphaBlendDest,
                                       BlendState::BlendOperation newAlphaOperation,
                                       uint8_t newColorMask)
        {
            enableBlending = newEnableBlending;
            colorBlendSource = newColorBlendSource;
            colorBlendDest = newColorBlendDest;
            colorOperation = newColorOperation;
            alphaBlendSource = newAlphaBlendSource;
            alphaBlendDest = newAlphaBlendDest;
            alphaOperation = newAlphaOperation;
            colorMask = newColorMask;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
