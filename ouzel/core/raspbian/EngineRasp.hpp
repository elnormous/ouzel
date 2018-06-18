// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineRasp: public Engine
    {
    public:
        EngineRasp(int argc, char* argv[]);

        virtual void run() override;

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;

    protected:
        void executeAll();

        std::queue<std::function<void(void)>> executeQueue;
        Mutex executeMutex;
    };
}
