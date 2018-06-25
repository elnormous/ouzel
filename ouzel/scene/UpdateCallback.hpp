// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include <cstdint>
#include "scene/UpdateCallback.hpp"

namespace ouzel
{
    namespace scene
    {
        class SceneManager;

        class UpdateCallback
        {
            friend SceneManager;
        public:
            static const int32_t PRIORITY_MAX = 0x1000;

            explicit UpdateCallback(int32_t initPriority = 0): priority(initPriority) {}
            ~UpdateCallback();

            void remove();

            std::function<void(float)> callback;
            float interval = 0.0F;

        private:
            int32_t priority;
            SceneManager* sceneManager = nullptr;
            float timeSinceLastUpdate = 0.0F;
        };
    }
}
