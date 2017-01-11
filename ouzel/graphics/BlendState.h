// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BlendState: public Resource, public Noncopyable
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
            virtual void free() override;

            virtual bool init(bool newEnableBlending,
                              BlendFactor newColorBlendSource, BlendFactor newColorBlendDest,
                              BlendOperation newColorOperation,
                              BlendFactor newAlphaBlendSource, BlendFactor newAlphaBlendDest,
                              BlendOperation newAlphaOperation);

            bool isBlendingEnabled() const { return data.enableBlending; }
            BlendFactor getColorBlendSource() const { return data.colorBlendSource; }
            BlendFactor getColorBlendDest() const { return data.colorBlendDest; }
            BlendOperation getColorOperation() const { return data.colorOperation; }
            BlendFactor getAlphaBlendSource() const { return data.alphaBlendSource; }
            BlendFactor getAlphaBlendDest() const { return data.alphaBlendDest; }
            BlendOperation getAlphaOperation() const { return data.alphaOperation; }

        protected:
            BlendState();
            virtual void update() override;
            virtual bool upload() override;

            struct Data
            {
                BlendFactor colorBlendSource = BlendFactor::ONE;
                BlendFactor colorBlendDest = BlendFactor::ZERO;
                BlendOperation colorOperation = BlendOperation::ADD;
                BlendFactor alphaBlendSource = BlendFactor::ONE;
                BlendFactor alphaBlendDest = BlendFactor::ZERO;
                BlendOperation alphaOperation = BlendOperation::ADD;
                bool enableBlending = false;
            };

            Data uploadData;
            std::mutex uploadMutex;

        private:
            Data data;

            Data currentData;
        };
    } // namespace graphics
} // namespace ouzel
