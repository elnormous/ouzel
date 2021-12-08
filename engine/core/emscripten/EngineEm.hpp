// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_ENGINEEM_HPP
#define OUZEL_CORE_ENGINEEM_HPP

#include "../Engine.hpp"

namespace ouzel::core::emscripten
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

#endif // OUZEL_CORE_ENGINEEM_HPP
