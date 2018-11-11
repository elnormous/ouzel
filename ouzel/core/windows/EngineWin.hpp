// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINEWIN_HPP
#define OUZEL_CORE_ENGINEWIN_HPP

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
        ~EngineWin();

        void run();

        void executeOnMainThread(const std::function<void()>& func) override;
        void openURL(const std::string& url) override;

        void executeAll();

    private:
        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;
    };
}

#endif // OUZEL_CORE_ENGINEWIN_HPP
