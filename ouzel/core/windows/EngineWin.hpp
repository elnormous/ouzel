// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include "core/Engine.hpp"

namespace ouzel
{
    class EngineWin final: public Engine
    {
    public:
        EngineWin(int initArgc, LPWSTR* initArgv);
        virtual ~EngineWin();

        void run();

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;
        virtual void openURL(const std::string& url) override;

        void executeAll();

    private:
        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;
    };
}
