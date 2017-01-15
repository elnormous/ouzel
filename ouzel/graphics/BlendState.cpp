// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendState.h"
#include "Renderer.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace graphics
    {
        BlendState::BlendState()
        {
        }

        BlendState::~BlendState()
        {
        }

        void BlendState::free()
        {
        }

        bool BlendState::init(bool newEnableBlending,
                              BlendFactor newColorBlendSource, BlendFactor newColorBlendDest,
                              BlendOperation newColorOperation,
                              BlendFactor newAlphaBlendSource, BlendFactor newAlphaBlendDest,
                              BlendOperation newAlphaOperation)
        {
            data.enableBlending = newEnableBlending;
            data.colorBlendSource = newColorBlendSource;
            data.colorBlendDest = newColorBlendDest;
            data.colorOperation = newColorOperation;
            data.alphaBlendSource = newAlphaBlendSource;
            data.alphaBlendDest = newAlphaBlendDest;
            data.alphaOperation = newAlphaOperation;

            update();

            return true;
        }

        void BlendState::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            currentData = data;

            dirty = true;
        }

        bool BlendState::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dirty = false;
            uploadData = std::move(currentData);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
