// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Noncopyable.h"

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

            virtual bool init(bool enableBlending,
                              BlendFactor colorBlendSource, BlendFactor colorBlendDest,
                              BlendOperation colorOperation,
                              BlendFactor alphaBlendSource, BlendFactor alphaBlendDest,
                              BlendOperation alphaOperation);

            bool isBlendingEnabled() const { return _enableBlending; }
            BlendFactor getColorBlendSource() const { return _colorBlendSource; }
            BlendFactor getColorBlendDest() const { return _colorBlendDest; }
            BlendOperation getColorOperation() const { return _colorOperation; }
            BlendFactor getAlphaBlendSource() const { return _alphaBlendSource; }
            BlendFactor getAlphaBlendDest() const { return _alphaBlendDest; }
            BlendOperation getAlphaOperation() const { return _alphaOperation; }

        protected:
            BlendState();

            bool _enableBlending = false;
            BlendFactor _colorBlendSource = BlendFactor::ONE;
            BlendFactor _colorBlendDest = BlendFactor::ZERO;
            BlendOperation _colorOperation = BlendOperation::ADD;
            BlendFactor _alphaBlendSource = BlendFactor::ONE;
            BlendFactor _alphaBlendDest = BlendFactor::ZERO;
            BlendOperation _alphaOperation = BlendOperation::ADD;

        };
    } // namespace graphics
} // namespace ouzel
