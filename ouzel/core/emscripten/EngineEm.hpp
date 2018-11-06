// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef ENGINEEM_HPP
#define ENGINEEM_HPP

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineEm final: public Engine
    {
    public:
        EngineEm(int argc, char* argv[]);

        void run();

        void step();

        void executeOnMainThread(const std::function<void(void)>& func) override;
        void openURL(const std::string& url) override;
    };
}

#endif // ENGINEEM_HPP
