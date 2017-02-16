// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateResource.h"

namespace ouzel
{
    namespace graphics
    {
        BlendStateResource::BlendStateResource()
        {
        }

        BlendStateResource::~BlendStateResource()
        {
        }

        bool BlendStateResource::init(bool newEnableBlending,
                                      BlendState::BlendFactor newColorBlendSource, BlendState::BlendFactor newColorBlendDest,
                                      BlendState::BlendOperation newColorOperation,
                                      BlendState::BlendFactor newAlphaBlendSource, BlendState::BlendFactor newAlphaBlendDest,
                                      BlendState::BlendOperation newAlphaOperation)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            enableBlending = newEnableBlending;
            colorBlendSource = newColorBlendSource;
            colorBlendDest = newColorBlendDest;
            colorOperation = newColorOperation;
            alphaBlendSource = newAlphaBlendSource;
            alphaBlendDest = newAlphaBlendDest;
            alphaOperation = newAlphaOperation;

            dirty |= 0x01;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
