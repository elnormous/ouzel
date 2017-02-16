// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class BlendStateResource;

        class BlendState: public Noncopyable
        {
        public:
            enum class BlendFactor
            {
                ZERO,
                ONE,
                SRC_COLOR,
                INV_SRC_COLOR,
                SRC_ALPHA,
                INV_SRC_ALPHA,
                DEST_ALPHA,
                INV_DEST_ALPHA,
                DEST_COLOR,
                INV_DEST_COLOR,
                SRC_ALPHA_SAT,
                BLEND_FACTOR,
                INV_BLEND_FACTOR
            };

            enum class BlendOperation
            {
                ADD,
                SUBTRACT,
                REV_SUBTRACT,
                MIN,
                MAX
            };

            BlendState();
            virtual ~BlendState();

            bool init(bool newEnableBlending,
                      BlendState::BlendFactor newColorBlendSource, BlendState::BlendFactor newColorBlendDest,
                      BlendState::BlendOperation newColorOperation,
                      BlendState::BlendFactor newAlphaBlendSource, BlendState::BlendFactor newAlphaBlendDest,
                      BlendState::BlendOperation newAlphaOperation);

            BlendStateResource* getResource() const { return resource; }

            bool isBlendingEnabled() const { return enableBlending; }
            BlendState::BlendFactor getColorBlendSource() const { return colorBlendSource; }
            BlendState::BlendFactor getColorBlendDest() const { return colorBlendDest; }
            BlendState::BlendOperation getColorOperation() const { return colorOperation; }
            BlendState::BlendFactor getAlphaBlendSource() const { return alphaBlendSource; }
            BlendState::BlendFactor getAlphaBlendDest() const { return alphaBlendDest; }
            BlendState::BlendOperation getAlphaOperation() const { return alphaOperation; }

        private:
            BlendStateResource* resource = nullptr;

            BlendState::BlendFactor colorBlendSource = BlendState::BlendFactor::ONE;
            BlendState::BlendFactor colorBlendDest = BlendState::BlendFactor::ZERO;
            BlendState::BlendOperation colorOperation = BlendState::BlendOperation::ADD;
            BlendState::BlendFactor alphaBlendSource = BlendState::BlendFactor::ONE;
            BlendState::BlendFactor alphaBlendDest = BlendState::BlendFactor::ZERO;
            BlendState::BlendOperation alphaOperation = BlendState::BlendOperation::ADD;
            bool enableBlending = false;
        };
    } // namespace graphics
} // namespace ouzel
