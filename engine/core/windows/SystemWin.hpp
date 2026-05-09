#ifndef FLUXION_CORE_SYSTEMWIN_HPP
#define FLUXION_CORE_SYSTEMWIN_HPP

#include "../System.hpp"
#include "EngineWin.hpp"

namespace fluxion::core::windows
{
    class System final: public core::System
    {
    public:
        System(int argc, LPWSTR* argv);
        ~System() override = default;

        int run();

    private:
        Engine engine;
    };
}

#endif // FLUXION_CORE_SYSTEMWIN_HPP
