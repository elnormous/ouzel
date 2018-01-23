// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineRasp: public Engine
    {
    public:
        EngineRasp(int aArgc, char* aArgv[]);

        virtual int run() override;

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;

        virtual bool setCurrentThreadName(const std::string& name) override;

    protected:
        void executeAll();

        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;
    };
}
