// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Egnine;

    class UpdateCallback
    {
        friend Engine;
    public:
        std::function<void(float)> callback;
    protected:
        bool _remove = false;
    };
}
