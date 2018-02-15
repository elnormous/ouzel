// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineLinux: public Engine
    {
    public:
        EngineLinux(int initArgc, char* initArgv[]);

        virtual int run() override;

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;
        virtual bool openURL(const std::string& url) override;

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

        int getArgc() const { return argc; }
        char** getArgv() const { return argv; }

    protected:
        void executeAll();

        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;

        int argc = 0;
        char** argv = nullptr;
    };
}
