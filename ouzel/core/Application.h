// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <string>
#include <queue>
#include <functional>
#include <mutex>
#include "core/Engine.h"
#include "files/FileSystem.h"
#include "utils/Noncopyable.h"

namespace ouzel
{
    class Application: public Noncopyable
    {
    public:
        Application();
        Application(int aArgc, char* aArgv[]);
        Application(const std::vector<std::string>& aArgs);
        virtual ~Application();
        bool init();

        virtual int run();
        virtual void exit();
        bool isActive() const { return active; }

        int getArgc() const { return argc; }
        char** getArgv() const { return argv; }
        const std::vector<std::string>& getArgs() { return args; }

        virtual void execute(const std::function<void(void)>& func) = 0;

        FileSystem* getFileSystem() { return &fileSystem; }

        virtual bool openURL(const std::string& url);

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled);
        bool isScreenSaverEnabled() const { return screenSaverEnabled; }

    protected:
        bool active = true;
        bool screenSaverEnabled = true;
        int argc = 0;
        char** argv = nullptr;
        std::vector<std::string> args;

        FileSystem fileSystem;
        Engine engine;
    };

    extern Application* sharedApplication;
}
