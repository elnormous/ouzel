// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendState.hpp"
#include "BlendStateResource.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace graphics
    {
        BlendState::BlendState()
        {
            resource = engine->getRenderer()->getDevice()->createBlendState();
        }

        BlendState::~BlendState()
        {
            if (engine && resource) engine->getRenderer()->getDevice()->deleteResource(resource);
        }

        bool BlendState::init(bool newEnableBlending,
                              BlendFactor newColorBlendSource, BlendFactor newColorBlendDest,
                              BlendOperation newColorOperation,
                              BlendFactor newAlphaBlendSource, BlendFactor newAlphaBlendDest,
                              BlendOperation newAlphaOperation,
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

            engine->getRenderer()->executeOnRenderThread(std::bind(&BlendStateResource::init,
                                                                         resource,
                                                                         newEnableBlending,
                                                                         newColorBlendSource, newColorBlendDest,
                                                                         newColorOperation,
                                                                         newAlphaBlendSource, newAlphaBlendDest,
                                                                         newAlphaOperation,
                                                                         newColorMask));

            return true;
        }

        bool BlendState::isBlendingEnabled() const
        {
            return enableBlending;
        }

        BlendState::BlendFactor BlendState::getColorBlendSource() const
        {
            return colorBlendSource;
        }

        BlendState::BlendFactor BlendState::getColorBlendDest() const
        {
            return colorBlendDest;
        }

        BlendState::BlendOperation BlendState::getColorOperation() const
        {
            return colorOperation;
        }

        BlendState::BlendFactor BlendState::getAlphaBlendSource() const
        {
            return alphaBlendSource;
        }

        BlendState::BlendFactor BlendState::getAlphaBlendDest() const
        {
            return alphaBlendDest;
        }

        BlendState::BlendOperation BlendState::getAlphaOperation() const
        {
            return alphaOperation;
        }

        uint8_t BlendState::getColorMask() const
        {
            return colorMask;
        }
    } // namespace graphics
} // namespace ouzel
