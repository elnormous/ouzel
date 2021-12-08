// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_SYSTEMWIN_HPP
#define OUZEL_CORE_SYSTEMWIN_HPP

#include "../System.hpp"
#include "EngineWin.hpp"

namespace ouzel::core::windows
{
    class System final: public core::System
    {
    public:
        System(int argc, LPWSTR* argv);
        ~System() override = default;

        void run();

    private:
        Engine engine;
    };
}

#endif // OUZEL_CORE_SYSTEMWIN_HPP
