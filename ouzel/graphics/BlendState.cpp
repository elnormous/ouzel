// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendState.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"

namespace ouzel
{
    namespace graphics
    {
        BlendState::BlendState()
        {
        }

        BlendState::~BlendState()
        {
        }

        void BlendState::free()
        {
            ready = false;
        }

        bool BlendState::init(bool newEnableBlending,
                              BlendFactor newColorBlendSource, BlendFactor newColorBlendDest,
                              BlendOperation newColorOperation,
                              BlendFactor newAlphaBlendSource, BlendFactor newAlphaBlendDest,
                              BlendOperation newAlphaOperation)
        {
            data.enableBlending = newEnableBlending;
            data.colorBlendSource = newColorBlendSource;
            data.colorBlendDest = newColorBlendDest;
            data.colorOperation = newColorOperation;
            data.alphaBlendSource = newAlphaBlendSource;
            data.alphaBlendDest = newAlphaBlendDest;
            data.alphaOperation = newAlphaOperation;

            dirty = true;
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        void BlendState::update()
        {
            uploadData = data;
        }

        bool BlendState::upload()
        {
            ready = true;
            dirty = false;
            
            return true;
        }
    } // namespace graphics
} // namespace ouzel
