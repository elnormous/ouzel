// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_ENGINETVOS_HPP
#define OUZEL_CORE_ENGINETVOS_HPP

#ifdef __OBJC__
@class ExecuteHandler;
using ExecuteHandlerPtr = ExecuteHandler*;
#else
#  include <objc/NSObjCRuntime.h>
using ExecuteHandlerPtr = id;
#endif

#include "../Engine.hpp"

namespace ouzel::core::tvos
{
    class Engine final: public core::Engine
    {
    public:
        Engine(const std::vector<std::string>& args);
        ~Engine() override;

        void openUrl(const std::string& url) final;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) final;

        void executeAll();

    private:
        void runOnMainThread(const std::function<void()>& func) final;

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;

        ExecuteHandlerPtr executeHanlder = nil;
    };
}

#endif // OUZEL_CORE_ENGINETVOS_HPP
