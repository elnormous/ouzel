// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>

namespace ouzel
{
    namespace graphics
    {
        class BlendStateResource;

        class BlendState
        {
        public:
            enum class Factor
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

            enum class Operation
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

            BlendState(const BlendState&) = delete;
            BlendState& operator=(const BlendState&) = delete;

            BlendState(const BlendState&&) = delete;
            BlendState& operator=(const BlendState&&) = delete;

            bool init(bool newEnableBlending,
                      Factor newColorBlendSource, Factor newColorBlendDest,
                      Operation newColorOperation,
                      Factor newAlphaBlendSource, Factor newAlphaBlendDest,
                      Operation newAlphaOperation,
                      uint8_t newColorMask = COLOR_MASK_ALL);

            BlendStateResource* getResource() const { return resource; }

            bool isBlendingEnabled() const;
            Factor getColorBlendSource() const;
            Factor getColorBlendDest() const;
            Operation getColorOperation() const;
            Factor getAlphaBlendSource() const;
            Factor getAlphaBlendDest() const;
            Operation getAlphaOperation() const;
            uint8_t getColorMask() const;

        private:
            BlendStateResource* resource = nullptr;

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
