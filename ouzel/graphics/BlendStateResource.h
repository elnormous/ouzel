// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <mutex>
#include "utils/Noncopyable.h"
#include "graphics/BlendState.h"
#include "graphics/Resource.h"

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class BlendStateResource: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~BlendStateResource();
            virtual void free() override;

            virtual bool init(bool newEnableBlending,
                              BlendState::BlendFactor newColorBlendSource, BlendState::BlendFactor newColorBlendDest,
                              BlendState::BlendOperation newColorOperation,
                              BlendState::BlendFactor newAlphaBlendSource, BlendState::BlendFactor newAlphaBlendDest,
                              BlendState::BlendOperation newAlphaOperation);

            bool isBlendingEnabled() const { return data.enableBlending; }
            BlendState::BlendFactor getColorBlendSource() const { return data.colorBlendSource; }
            BlendState::BlendFactor getColorBlendDest() const { return data.colorBlendDest; }
            BlendState::BlendOperation getColorOperation() const { return data.colorOperation; }
            BlendState::BlendFactor getAlphaBlendSource() const { return data.alphaBlendSource; }
            BlendState::BlendFactor getAlphaBlendDest() const { return data.alphaBlendDest; }
            BlendState::BlendOperation getAlphaOperation() const { return data.alphaOperation; }

        protected:
            BlendStateResource();
            virtual void update() override;
            virtual bool upload() override;

            struct Data
            {
                BlendState::BlendFactor colorBlendSource = BlendState::BlendFactor::ONE;
                BlendState::BlendFactor colorBlendDest = BlendState::BlendFactor::ZERO;
                BlendState::BlendOperation colorOperation = BlendState::BlendOperation::ADD;
                BlendState::BlendFactor alphaBlendSource = BlendState::BlendFactor::ONE;
                BlendState::BlendFactor alphaBlendDest = BlendState::BlendFactor::ZERO;
                BlendState::BlendOperation alphaOperation = BlendState::BlendOperation::ADD;
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
