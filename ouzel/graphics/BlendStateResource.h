// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "graphics/BlendState.h"

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BlendStateResource: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~BlendStateResource();

            bool init(bool newEnableBlending,
                      BlendState::BlendFactor newColorBlendSource, BlendState::BlendFactor newColorBlendDest,
                      BlendState::BlendOperation newColorOperation,
                      BlendState::BlendFactor newAlphaBlendSource, BlendState::BlendFactor newAlphaBlendDest,
                      BlendState::BlendOperation newAlphaOperation,
                      uint8_t newColorMask);

            bool isBlendingEnabled() const { return enableBlending; }
            BlendState::BlendFactor getColorBlendSource() const { return colorBlendSource; }
            BlendState::BlendFactor getColorBlendDest() const { return colorBlendDest; }
            BlendState::BlendOperation getColorOperation() const { return colorOperation; }
            BlendState::BlendFactor getAlphaBlendSource() const { return alphaBlendSource; }
            BlendState::BlendFactor getAlphaBlendDest() const { return alphaBlendDest; }
            BlendState::BlendOperation getAlphaOperation() const { return alphaOperation; }
            uint8_t getColorMask() const { return colorMask; }

        protected:
            BlendStateResource();

            uint32_t dirty = 0;
            BlendState::BlendFactor colorBlendSource = BlendState::BlendFactor::ONE;
            BlendState::BlendFactor colorBlendDest = BlendState::BlendFactor::ZERO;
            BlendState::BlendOperation colorOperation = BlendState::BlendOperation::ADD;
            BlendState::BlendFactor alphaBlendSource = BlendState::BlendFactor::ONE;
            BlendState::BlendFactor alphaBlendDest = BlendState::BlendFactor::ZERO;
            BlendState::BlendOperation alphaOperation = BlendState::BlendOperation::ADD;
            uint8_t colorMask = BlendState::COLOR_MASK_ALL;
            bool enableBlending = false;

            std::mutex uploadMutex;
        };
    } // namespace graphics
} // namespace ouzel
