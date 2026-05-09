#ifndef FLUXION_CORE_SYSTEMMACOS_HPP
#define FLUXION_CORE_SYSTEMMACOS_HPP

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

namespace fluxion::core::macos
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

#endif // FLUXION_CORE_SYSTEMMACOS_HPP
