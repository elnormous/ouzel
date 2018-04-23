// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include <cstdint>

namespace ouzel
{
    class Engine;

    class UpdateCallback
    {
        friend Engine;
    public:
        static const int32_t PRIORITY_MAX = 0x1000;

        explicit UpdateCallback(int32_t initPriority = 0): priority(initPriority) {}
        ~UpdateCallback();

        void remove();

        std::function<void(float)> callback;
        float interval = 0.0F;

    private:
        int32_t priority;
        Engine* engine = nullptr;
        float timeSinceLastUpdate = 0.0F;
    };
}
