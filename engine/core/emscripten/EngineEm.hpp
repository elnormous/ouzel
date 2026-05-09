#ifndef FLUXION_CORE_ENGINEEM_HPP
#define FLUXION_CORE_ENGINEEM_HPP

#include "../Engine.hpp"

namespace fluxion::core::emscripten
{
    class Engine final: public core::Engine
    {
    public:
        Engine(const std::vector<std::string>& args);

        void run();

        void step();
        void handleOrientationChange(int orientation);

        void openUrl(const std::string& url) final;

    private:
        void runOnMainThread(const std::function<void()>& func) final;
    };
}

#endif // FLUXION_CORE_ENGINEEM_HPP
