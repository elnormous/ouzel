// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include <set>
#include "Types.h"
#include "Noncopyable.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "FileSystem.h"
#include "Input.h"
#include "EventDispatcher.h"
#include "UpdateCallback.h"
#include "Settings.h"

void ouzelMain(const std::vector<std::string>& args);

namespace ouzel
{
    extern Engine* sharedEngine;

    class Engine: public Noncopyable
    {
    public:
        Engine();
        virtual ~Engine();

        static std::set<graphics::Renderer::Driver> getAvailableDrivers();

        bool init(Settings& newSettings);
        const Settings& getSettings() const { return settings; }

        const EventDispatcherPtr& getEventDispatcher() const { return eventDispatcher; }
        const CachePtr& getCache() const { return cache; }
        const WindowPtr& getWindow() const { return window; }
        const graphics::RendererPtr& getRenderer() const { return renderer; }
        const scene::SceneManagerPtr& getSceneManager() const { return sceneManager; }
        const FileSystemPtr& getFileSystem() const { return fileSystem; }
        const input::InputPtr& getInput() const { return input; }
        const LocalizationPtr& getLocalization() const { return localization; }

        void exit();

        void begin();
        void end();
        bool run();

        float getTargetFPS() const { return targetFPS; }
        float getFPS() const { return currentFPS; }

        void scheduleUpdate(const UpdateCallbackPtr& callback);
        void unscheduleUpdate(const UpdateCallbackPtr& callback);

    protected:
        Settings settings;

        EventDispatcherPtr eventDispatcher;
        input::InputPtr input;
        CachePtr cache;
        WindowPtr window;
        FileSystemPtr fileSystem;
        LocalizationPtr localization;
        graphics::RendererPtr renderer;
        scene::SceneManagerPtr sceneManager;

        float targetFPS;
        uint64_t targetFrameInterval;
        float currentFPS = 0.0f;
        uint64_t previousFrameTime;

        std::vector<UpdateCallbackPtr> updateCallbacks;
        std::set<UpdateCallbackPtr> updateCallbackAddList;
        std::set<UpdateCallbackPtr> updateCallbackRemoveList;

        bool running = false;
        bool active = true;
    };
}
