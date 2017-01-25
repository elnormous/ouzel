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
            
            pendingData.enableBlending = newEnableBlending;
            pendingData.colorBlendSource = newColorBlendSource;
            pendingData.colorBlendDest = newColorBlendDest;
            pendingData.colorOperation = newColorOperation;
            pendingData.alphaBlendSource = newAlphaBlendSource;
            pendingData.alphaBlendDest = newAlphaBlendDest;
            pendingData.alphaOperation = newAlphaOperation;

            pendingData.dirty |= 0x01;

            return true;
        }

        bool BlendStateResource::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            data.dirty |= pendingData.dirty;
            pendingData.dirty = 0;

            if (data.dirty)
            {
                data.colorBlendSource = pendingData.colorBlendSource;
                data.colorBlendDest = pendingData.colorBlendDest;
                data.colorOperation = pendingData.colorOperation;
                data.alphaBlendSource = pendingData.alphaBlendSource;
                data.alphaBlendDest = pendingData.alphaBlendDest;
                data.alphaOperation = pendingData.alphaOperation;
                data.enableBlending = pendingData.enableBlending;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
