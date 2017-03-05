// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <string>
#include <queue>
#include <functional>
#include <mutex>
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

        virtual int run();
        virtual void exit();
        bool isActive() const { return active; }

        int getArgc() const { return argc; }
        char** getArgv() const { return argv; }
        const std::vector<std::string>& getArgs() { return args; }

        virtual void execute(const std::function<void(void)>& func);

        FileSystem* getFileSystem() { return &fileSystem; }

        virtual bool openURL(const std::string& url);

    protected:
        void executeAll();

        bool active = true;
        int argc = 0;
        char** argv = nullptr;
        std::vector<std::string> args;

        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;

        FileSystem fileSystem;
    };

    extern Application* sharedApplication;
}
