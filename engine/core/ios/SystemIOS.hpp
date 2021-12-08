// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_SYSTEMIOS_HPP
#define OUZEL_CORE_SYSTEMIOS_HPP

#include "../System.hpp"
#include "EngineIOS.hpp"

namespace ouzel::core::ios
{
    class System final: public core::System
    {
    public:
        System(int argc, char* argv[]);
        ~System() override = default;

        int run();

        auto getArgumentCount() const { return argumentCount; }
        auto getArguments() const { return arguments; }

        void start();

    private:
        int argumentCount;
        char** arguments;

        std::unique_ptr<ouzel::core::ios::Engine> engine;
    };
}

#endif // OUZEL_CORE_SYSTEMIOS_HPP
