// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_SYSTEMMACOS_HPP
#define OUZEL_CORE_SYSTEMMACOS_HPP

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
using NSApplicationPtr = NSApplication*;
using NSApplicationDelegatePtr = id<NSApplicationDelegate>;
#else
#  include <objc/NSObjCRuntime.h>
using NSApplicationPtr = id;
using NSApplicationDelegatePtr = id;
#endif
#include "../System.hpp"
#include "EngineMacOS.hpp"

namespace ouzel::core::macos
{
    class System final: public core::System
    {
    public:
        System(int argc, char* argv[]);
        ~System() override;

        void run();

        auto getArgumentCount() const { return argumentCount; }
        auto getArguments() const { return arguments; }

        void start();

    private:
        int argumentCount;
        char** arguments;

        NSApplicationDelegatePtr appDelegate = nil;
        std::unique_ptr<Engine> engine;
    };
}

#endif // OUZEL_CORE_SYSTEMMACOS_HPP
