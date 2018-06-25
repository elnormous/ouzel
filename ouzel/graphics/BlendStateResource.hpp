// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdint>
#include "graphics/RenderResource.hpp"
#include "graphics/BlendState.hpp"

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BlendStateResource: public RenderResource
        {
            friend Renderer;
        public:
            virtual ~BlendStateResource();

            virtual void init(bool newEnableBlending,
                              BlendState::Factor newColorBlendSource, BlendState::Factor newColorBlendDest,
                              BlendState::Operation newColorOperation,
                              BlendState::Factor newAlphaBlendSource, BlendState::Factor newAlphaBlendDest,
                              BlendState::Operation newAlphaOperation,
                              uint8_t newColorMask);

            bool isBlendingEnabled() const { return enableBlending; }
            BlendState::Factor getColorBlendSource() const { return colorBlendSource; }
            BlendState::Factor getColorBlendDest() const { return colorBlendDest; }
            BlendState::Operation getColorOperation() const { return colorOperation; }
            BlendState::Factor getAlphaBlendSource() const { return alphaBlendSource; }
            BlendState::Factor getAlphaBlendDest() const { return alphaBlendDest; }
            BlendState::Operation getAlphaOperation() const { return alphaOperation; }
            uint8_t getColorMask() const { return colorMask; }

        protected:
            BlendStateResource();

            BlendState::Factor colorBlendSource = BlendState::Factor::ONE;
            BlendState::Factor colorBlendDest = BlendState::Factor::ZERO;
            BlendState::Operation colorOperation = BlendState::Operation::ADD;
            BlendState::Factor alphaBlendSource = BlendState::Factor::ONE;
            BlendState::Factor alphaBlendDest = BlendState::Factor::ZERO;
            BlendState::Operation alphaOperation = BlendState::Operation::ADD;
            uint8_t colorMask = BlendState::COLOR_MASK_ALL;
            bool enableBlending = false;
        };
    } // namespace graphics
} // namespace ouzel
