// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendState.h"
#include "BlendStateResource.h"
#include "Renderer.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace graphics
    {
        BlendState::BlendState()
        {
            resource = sharedEngine->getRenderer()->createBlendState();
        }

        BlendState::~BlendState()
        {
            if (resource) sharedEngine->getRenderer()->deleteResource(resource);
        }

        bool BlendState::init(bool newEnableBlending,
                              BlendState::BlendFactor newColorBlendSource, BlendState::BlendFactor newColorBlendDest,
                              BlendState::BlendOperation newColorOperation,
                              BlendState::BlendFactor newAlphaBlendSource, BlendState::BlendFactor newAlphaBlendDest,
                              BlendState::BlendOperation newAlphaOperation)
        {
            enableBlending = newEnableBlending;
            colorBlendSource = newColorBlendSource;
            colorBlendDest = newColorBlendDest;
            colorOperation = newColorOperation;
            alphaBlendSource = newAlphaBlendSource;
            alphaBlendDest = newAlphaBlendDest;
            alphaOperation = newAlphaOperation;

            if (!resource->init(newEnableBlending,
                                newColorBlendSource, newColorBlendDest,
                                newColorOperation,
                                newAlphaBlendSource, newAlphaBlendDest,
                                newAlphaOperation))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
