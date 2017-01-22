// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BlendStateResource.h"
#include "Renderer.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace graphics
    {
        BlendStateResource::BlendStateResource()
        {
        }

        BlendStateResource::~BlendStateResource()
        {
        }

        void BlendStateResource::free()
        {
        }

        bool BlendStateResource::init(bool newEnableBlending,
                                      BlendState::BlendFactor newColorBlendSource, BlendState::BlendFactor newColorBlendDest,
                                      BlendState::BlendOperation newColorOperation,
                                      BlendState::BlendFactor newAlphaBlendSource, BlendState::BlendFactor newAlphaBlendDest,
                                      BlendState::BlendOperation newAlphaOperation)
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

        void BlendStateResource::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            currentData = data;

            dirty = true;
        }

        bool BlendStateResource::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dirty = false;
            uploadData = std::move(currentData);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
