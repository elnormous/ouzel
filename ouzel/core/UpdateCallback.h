// Copyright (C) 2017 Elviss Strazdins
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

        UpdateCallback(int32_t aPriority = 0): priority(aPriority) {}
        ~UpdateCallback();

        void remove();

        std::function<void(float)> callback;
        float interval = 0.0f;

    protected:
        int32_t priority;
        Engine* engine = nullptr;
        float timeSinceLastUpdate = 0.0f;
    };
}
