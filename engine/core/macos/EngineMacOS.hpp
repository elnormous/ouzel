// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_ENGINEMACOS_HPP
#define OUZEL_CORE_ENGINEMACOS_HPP

#include <IOKit/pwr_mgt/IOPMLib.h>

#include "../Engine.hpp"

namespace ouzel::core::macos
{
    class Engine final: public core::Engine
    {
    public:
        Engine(const std::vector<std::string>& args);
        ~Engine() override;

        void run();

        void openUrl(const std::string& url) final;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) final;

        void executeAll();

    private:
        void engineMain() final;
        void runOnMainThread(const std::function<void()>& func) final;

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;

        IOPMAssertionID noSleepAssertionId = 0;
        id executeHanlder = nil;
    };
}

#endif // OUZEL_CORE_ENGINEMACOS_HPP
