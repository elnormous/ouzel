// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Application.h"

namespace ouzel
{
    class ApplicationLinux: public Application
    {
    public:
        ApplicationLinux(int aArgc, char* aArgv[]);

        virtual int run() override;

        virtual void execute(const std::function<void(void)>& func) override;
        virtual bool openURL(const std::string& url) override;

    protected:
        void executeAll();

        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;
    };
}
