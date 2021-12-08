// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_ENGINETVOS_HPP
#define OUZEL_CORE_ENGINETVOS_HPP

#ifdef __OBJC__
@class ExecuteHandler;
typedef ExecuteHandler* ExecuteHandlerPtr;
#else
#  include <objc/NSObjCRuntime.h>
typedef id ExecuteHandlerPtr;
#endif

#include "../Engine.hpp"

namespace ouzel::core::tvos
{
    class Engine final: public core::Engine
    {
    public:
        Engine(int argc, char* argv[]);
        ~Engine() override;

        void run();

        void openUrl(const std::string& url) final;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) final;

        void executeAll();

    private:
        void runOnMainThread(const std::function<void()>& func) final;

        int argumentCount;
        char** arguments;

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;

        ExecuteHandlerPtr executeHanlder = nil;
    };
}

#endif // OUZEL_CORE_ENGINETVOS_HPP
