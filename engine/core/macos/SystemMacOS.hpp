// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_SYSTEMMACOS_HPP
#define OUZEL_CORE_SYSTEMMACOS_HPP

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
using NSApplicationPtr = NSApplication*;
#else
#  include <objc/NSObjCRuntime.h>
using NSApplicationPtr = id;
#endif
#include "../System.hpp"
#include "EngineMacOS.hpp"

namespace ouzel::core::macos
{
    class System final: public core::System
    {
    public:
        System(int argc, char* argv[]);
        ~System() override = default;

        void run();

        auto getArgumentCount() const { return argumentCount; }
        auto getArguments() const { return arguments; }

        void start();

    private:
        int argumentCount;
        char** arguments;

        std::unique_ptr<Engine> engine;
    };
}

#endif // OUZEL_CORE_SYSTEMMACOS_HPP
