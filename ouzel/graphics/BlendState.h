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
                ZERO              = 1,
                ONE               = 2,
                SRC_COLOR         = 3,
                INV_SRC_COLOR     = 4,
                SRC_ALPHA         = 5,
                INV_SRC_ALPHA     = 6,
                DEST_ALPHA        = 7,
                INV_DEST_ALPHA    = 8,
                DEST_COLOR        = 9,
                INV_DEST_COLOR    = 10,
                SRC_ALPHA_SAT     = 11,
                BLEND_FACTOR      = 14,
                INV_BLEND_FACTOR  = 15
            };

            enum class BlendOperation
            {
                ADD           = 1,
                SUBTRACT      = 2,
                REV_SUBTRACT  = 3,
                MIN           = 4,
                MAX           = 5
            };

            virtual ~BlendState() {}
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
