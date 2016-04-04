// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Noncopyable.h"

#pragma once

namespace ouzel
{
    namespace video
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
                INV_BLEND_FACTOR  = 15,
                SRC1_COLOR        = 16,
                INV_SRC1_COLOR    = 17,
                SRC1_ALPHA        = 18,
                INV_SRC1_ALPHA    = 19
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

        protected:
            BlendState();

            bool _enableBlending = false;
            BlendFactor _colorBlendSource;
            BlendFactor _colorBlendDest;
            BlendOperation _colorOperation;
            BlendFactor _alphaBlendSource;
            BlendFactor _alphaBlendDest;
            BlendOperation _alphaOperation;

        };
    } // namespace video
} // namespace ouzel
