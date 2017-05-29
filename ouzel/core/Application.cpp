// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "Application.h"
#include "Engine.h"
#include "CursorResource.h"

namespace ouzel
{
    ouzel::Application* sharedApplication = nullptr;

    Application::Application():
        screenSaverEnabled(false)
    {
        sharedApplication = this;
    }

    Application::Application(int aArgc, char* aArgv[]):
        argc(aArgc), argv(aArgv)
    {
        sharedApplication = this;

        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
    }

    Application::Application(const std::vector<std::string>& aArgs):
        args(aArgs)
    {
        sharedApplication = this;
    }

    Application::~Application()
    {
    }

    int Application::run()
    {
        return EXIT_SUCCESS;
    }

    void Application::exit()
    {
        active = false;

        if (sharedEngine)
        {
            sharedEngine->exit();
        }
    }

    void Application::execute(const std::function<void(void)>& func)
    {
        std::lock_guard<std::mutex> lock(executeMutex);

        executeQueue.push(func);
    }

    void Application::executeAll()
    {
        std::function<void(void)> func;

        for (;;)
        {
            {
                std::lock_guard<std::mutex> lock(executeMutex);

                if (executeQueue.empty())
                {
                    break;
                }

                func = executeQueue.front();
                executeQueue.pop();
            }

            if (func)
            {
                func();
            }
        }
    }

    bool Application::openURL(const std::string&)
    {
        return false;
    }

    void Application::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        screenSaverEnabled = newScreenSaverEnabled;
    }

    void Application::setCurrentCursor(Cursor* cursor)
    {
        std::lock_guard<std::mutex> lock(resourceMutex);

        if (cursor)
        {
            currentCursor = cursor->getResource();
        }
        else
        {
            currentCursor = nullptr;
        }

        setCursorResource(currentCursor);
    }

    void Application::setCursorResource(CursorResource*)
    {
    }

    CursorResource* Application::createCursorResource()
    {
        std::lock_guard<std::mutex> lock(resourceMutex);

        std::unique_ptr<CursorResource> cursorResource(new CursorResource());
        CursorResource* result = cursorResource.get();

        resources.push_back(std::move(cursorResource));

        return result;
    }

    void Application::deleteCursorResource(CursorResource* resource)
    {
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            std::vector<std::unique_ptr<CursorResource>>::iterator i = std::find_if(resources.begin(), resources.end(), [resource](const std::unique_ptr<CursorResource>& ptr) {
                return ptr.get() == resource;
            });

            if (i != resources.end())
            {
                resourceDeleteSet.push_back(std::move(*i));
                resources.erase(i);
            }
        }

        if (resource == currentCursor) setCurrentCursor(nullptr);

        execute([this] {
            std::lock_guard<std::mutex> lock(resourceMutex);
            resourceDeleteSet.clear();
        });
    }
}
