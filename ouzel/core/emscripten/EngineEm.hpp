// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINEEM_HPP
#define OUZEL_CORE_ENGINEEM_HPP

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineEm final: public Engine
    {
    public:
        EngineEm(int argc, char* argv[]);

        void run();

        void step();

        void openUrl(const std::string& url) final;

    private:
        void runOnMainThread(const std::function<void()>& func) final;
    };
}

#endif // OUZEL_CORE_ENGINEEM_HPP
