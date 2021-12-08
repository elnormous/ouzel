// Ouzel by Elviss Strazdins

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

#include "../Engine.hpp"
#include "../../platform/winapi/Com.hpp"
#ifdef DEBUG
#  include "../../platform/winapi/Console.hpp"
#endif
namespace ouzel::core::windows
{
    class Engine final: public core::Engine
    {
    public:
        Engine(const std::vector<std::string>& args);

        void run();

        void openUrl(const std::string& url) final;

        void executeAll();

    private:
        void runOnMainThread(const std::function<void()>& func) final;

        platform::winapi::Com com;
#ifdef DEBUG
        platform::winapi::Console console;
#endif
        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;
    };
}

#endif // OUZEL_CORE_ENGINEWIN_HPP
