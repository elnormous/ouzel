// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "utils/Noncopyable.hpp"

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

            enum ColorMask
            {
                COLOR_MASK_RED = 0x01,
                COLOR_MASK_GREEN = 0x02,
                COLOR_MASK_BLUE = 0x04,
                COLOR_MASK_ALPHA = 0x08,
                COLOR_MASK_ALL = COLOR_MASK_RED | COLOR_MASK_GREEN | COLOR_MASK_BLUE | COLOR_MASK_ALPHA
            };

            BlendState();
            virtual ~BlendState();

            bool init(bool newEnableBlending,
                      BlendFactor newColorBlendSource, BlendFactor newColorBlendDest,
                      BlendOperation newColorOperation,
                      BlendFactor newAlphaBlendSource, BlendFactor newAlphaBlendDest,
                      BlendOperation newAlphaOperation,
                      uint8_t newColorMask = COLOR_MASK_ALL);

            BlendStateResource* getResource() const { return resource; }

            bool isBlendingEnabled() const;
            BlendFactor getColorBlendSource() const;
            BlendFactor getColorBlendDest() const;
            BlendOperation getColorOperation() const;
            BlendFactor getAlphaBlendSource() const;
            BlendFactor getAlphaBlendDest() const;
            BlendOperation getAlphaOperation() const;
            uint8_t getColorMask() const;

        private:
            BlendStateResource* resource = nullptr;

            BlendState::BlendFactor colorBlendSource = BlendState::BlendFactor::ONE;
            BlendState::BlendFactor colorBlendDest = BlendState::BlendFactor::ZERO;
            BlendState::BlendOperation colorOperation = BlendState::BlendOperation::ADD;
            BlendState::BlendFactor alphaBlendSource = BlendState::BlendFactor::ONE;
            BlendState::BlendFactor alphaBlendDest = BlendState::BlendFactor::ZERO;
            BlendState::BlendOperation alphaOperation = BlendState::BlendOperation::ADD;
            uint8_t colorMask = BlendState::COLOR_MASK_ALL;
            bool enableBlending = false;
        };
    } // namespace graphics
} // namespace ouzel
