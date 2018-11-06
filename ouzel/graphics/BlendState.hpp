// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BlendState final
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

            explicit BlendState(Renderer& initRenderer);
            BlendState(Renderer& initRenderer,
                       bool initEnableBlending,
                       Factor initColorBlendSource, Factor initColorBlendDest,
                       Operation initColorOperation,
                       Factor initAlphaBlendSource, Factor initAlphaBlendDest,
                       Operation initAlphaOperation,
                       uint8_t initColorMask = COLOR_MASK_ALL);
            ~BlendState();

            BlendState(const BlendState&) = delete;
            BlendState& operator=(const BlendState&) = delete;

            BlendState(BlendState&&) = delete;
            BlendState& operator=(BlendState&&) = delete;

            void init(bool newEnableBlending,
                      Factor newColorBlendSource, Factor newColorBlendDest,
                      Operation newColorOperation,
                      Factor newAlphaBlendSource, Factor newAlphaBlendDest,
                      Operation newAlphaOperation,
                      uint8_t newColorMask = COLOR_MASK_ALL);

            inline uintptr_t getResource() const { return resource; }

            inline bool isBlendingEnabled() const { return enableBlending; }

            inline BlendState::Factor getColorBlendSource() const { return colorBlendSource; }
            inline BlendState::Factor getColorBlendDest() const { return colorBlendDest; }
            inline BlendState::Operation getColorOperation() const { return colorOperation; }
            inline BlendState::Factor getAlphaBlendSource() const { return alphaBlendSource; }
            inline BlendState::Factor getAlphaBlendDest() const { return alphaBlendDest; }
            inline BlendState::Operation getAlphaOperation() const { return alphaOperation; }

            inline uint8_t getColorMask() const { return colorMask; }

        private:
            Renderer& renderer;
            uintptr_t resource = 0;

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
