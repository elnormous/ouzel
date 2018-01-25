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
                              Factor newColorBlendSource, Factor newColorBlendDest,
                              Operation newColorOperation,
                              Factor newAlphaBlendSource, Factor newAlphaBlendDest,
                              Operation newAlphaOperation,
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

        BlendState::Factor BlendState::getColorBlendSource() const
        {
            return colorBlendSource;
        }

        BlendState::Factor BlendState::getColorBlendDest() const
        {
            return colorBlendDest;
        }

        BlendState::Operation BlendState::getColorOperation() const
        {
            return colorOperation;
        }

        BlendState::Factor BlendState::getAlphaBlendSource() const
        {
            return alphaBlendSource;
        }

        BlendState::Factor BlendState::getAlphaBlendDest() const
        {
            return alphaBlendDest;
        }

        BlendState::Operation BlendState::getAlphaOperation() const
        {
            return alphaOperation;
        }

        uint8_t BlendState::getColorMask() const
        {
            return colorMask;
        }
    } // namespace graphics
} // namespace ouzel
