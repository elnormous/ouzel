// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "utils/Noncopyable.h"

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BlendState: public Noncopyable
        {
            friend Renderer;
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

            virtual ~BlendState();
            virtual void free();

            virtual bool init(bool newEnableBlending,
                              BlendFactor newColorBlendSource, BlendFactor newColorBlendDest,
                              BlendOperation newColorOperation,
                              BlendFactor newAlphaBlendSource, BlendFactor newAlphaBlendDest,
                              BlendOperation newAlphaOperation);

            bool isBlendingEnabled() const { return enableBlending; }
            BlendFactor getColorBlendSource() const { return colorBlendSource; }
            BlendFactor getColorBlendDest() const { return colorBlendDest; }
            BlendOperation getColorOperation() const { return colorOperation; }
            BlendFactor getAlphaBlendSource() const { return alphaBlendSource; }
            BlendFactor getAlphaBlendDest() const { return alphaBlendDest; }
            BlendOperation getAlphaOperation() const { return alphaOperation; }

            bool isReady() const { return ready; }

        protected:
            BlendState();

            bool enableBlending = false;
            BlendFactor colorBlendSource = BlendFactor::ONE;
            BlendFactor colorBlendDest = BlendFactor::ZERO;
            BlendOperation colorOperation = BlendOperation::ADD;
            BlendFactor alphaBlendSource = BlendFactor::ONE;
            BlendFactor alphaBlendDest = BlendFactor::ZERO;
            BlendOperation alphaOperation = BlendOperation::ADD;

            bool ready = false;
        };
    } // namespace graphics
} // namespace ouzel
