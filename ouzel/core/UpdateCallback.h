// Copyright (C) 2016 Elviss Strazdins
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

        UpdateCallback(int32_t aPriority = 0): priority(aPriority) { }
        ~UpdateCallback();

        std::function<void(float)> callback;

    protected:
        int32_t priority;
    };
}
