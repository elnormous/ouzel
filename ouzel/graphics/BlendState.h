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
                      BlendFactor newColorBlendSource, BlendFactor newColorBlendDest,
                      BlendOperation newColorOperation,
                      BlendFactor newAlphaBlendSource, BlendFactor newAlphaBlendDest,
                      BlendOperation newAlphaOperation);

            BlendStateResource* getResource() const { return resource; }

            bool isBlendingEnabled() const;
            BlendFactor getColorBlendSource() const;
            BlendFactor getColorBlendDest() const;
            BlendOperation getColorOperation() const;
            BlendFactor getAlphaBlendSource() const;
            BlendFactor getAlphaBlendDest() const;
            BlendOperation getAlphaOperation() const;

        private:
            BlendStateResource* resource = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
