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
            if (sharedEngine && resource) sharedEngine->getRenderer()->deleteResource(resource);
        }

        bool BlendState::init(bool newEnableBlending,
                              BlendFactor newColorBlendSource, BlendFactor newColorBlendDest,
                              BlendOperation newColorOperation,
                              BlendFactor newAlphaBlendSource, BlendFactor newAlphaBlendDest,
                              BlendOperation newAlphaOperation,
                              uint8_t newColorMask)
        {
            if (!resource->init(newEnableBlending,
                                newColorBlendSource, newColorBlendDest,
                                newColorOperation,
                                newAlphaBlendSource, newAlphaBlendDest,
                                newAlphaOperation,
                                newColorMask))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool BlendState::isBlendingEnabled() const
        {
            return resource->isBlendingEnabled();
        }

        BlendState::BlendFactor BlendState::getColorBlendSource() const
        {
            return resource->getColorBlendSource();
        }

        BlendState::BlendFactor BlendState::getColorBlendDest() const
        {
            return resource->getColorBlendDest();
        }

        BlendState::BlendOperation BlendState::getColorOperation() const
        {
            return resource->getColorOperation();
        }

        BlendState::BlendFactor BlendState::getAlphaBlendSource() const
        {
            return resource->getAlphaBlendSource();
        }

        BlendState::BlendFactor BlendState::getAlphaBlendDest() const
        {
            return resource->getAlphaBlendDest();
        }

        BlendState::BlendOperation BlendState::getAlphaOperation() const
        {
            return resource->getAlphaOperation();
        }

        uint8_t BlendState::getColorMask() const
        {
            return resource->getColorMask();
        }
    } // namespace graphics
} // namespace ouzel
