// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_SYSTEMTVOS_HPP
#define OUZEL_CORE_SYSTEMTVOS_HPP

#include "../System.hpp"
#include "EngineTVOS.hpp"

namespace ouzel::core::tvos
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

        std::unique_ptr<ouzel::core::tvos::Engine> engine;
    };
}

#endif // OUZEL_CORE_SYSTEMTVOS_HPP
