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

            RenderDevice* renderDevice = engine->getRenderer()->getDevice();

            return renderDevice->addCommand(InitBlendStateCommand(resource,
                                                                  newEnableBlending,
                                                                  newColorBlendSource, newColorBlendDest,
                                                                  newColorOperation,
                                                                  newAlphaBlendSource, newAlphaBlendDest,
                                                                  newAlphaOperation,
                                                                  newColorMask));
        }
    } // namespace graphics
} // namespace ouzel
