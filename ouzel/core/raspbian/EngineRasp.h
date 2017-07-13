// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Engine.h"

namespace ouzel
{
    class EngineRasp: public Engine
    {
    public:
        EngineRasp(int aArgc, char* aArgv[]);

        virtual int run() override;

        virtual void execute(const std::function<void(void)>& func) override;

    protected:
        void executeAll();

        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;
    };
}
