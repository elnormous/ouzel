// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINEWIN_HPP
#define OUZEL_CORE_ENGINEWIN_HPP

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineWin final: public Engine
    {
    public:
        EngineWin(int initArgc, LPWSTR* initArgv);
        ~EngineWin() override;

        void run();

        void openUrl(const std::string& url) final;

        void executeAll();

    private:
        void runOnMainThread(const std::function<void()>& func) final;

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;
    };
}

#endif // OUZEL_CORE_ENGINEWIN_HPP
