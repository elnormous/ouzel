// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <string>
#include "utils/Noncopyable.h"

namespace ouzel
{
    class Application: public Noncopyable
    {
    public:
        Application();
        Application(int pArgc, char* pArgv[]);
        Application(const std::vector<std::string>& pArgs);
        virtual ~Application();

        virtual bool run();

        int getArgc() const { return argc; }
        char** getArgv() const { return argv; }
        const std::vector<std::string>& getArgs() { return args; }

    protected:
        int argc = 0;
        char** argv = nullptr;
        std::vector<std::string> args;
    };

    extern Application* sharedApplication;
}
