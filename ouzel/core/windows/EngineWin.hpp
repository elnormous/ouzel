// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <Windows.h>
#include "core/Engine.hpp"

namespace ouzel
{
    class EngineWin : public Engine
    {
    public:
        EngineWin(int initArgc, LPWSTR* initArgv);
        virtual ~EngineWin();

        virtual void run() override;

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;
        virtual void openURL(const std::string& url) override;

        void executeAll();

    protected:
        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;
    };
}
