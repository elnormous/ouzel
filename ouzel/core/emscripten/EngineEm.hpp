// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineEm: public Engine
    {
    public:
        EngineEm(int argc, char* argv[]);

        virtual void run() override;

        bool step();

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;
        virtual void openURL(const std::string& url) override;
    };
}
