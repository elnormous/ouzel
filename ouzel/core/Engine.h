// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <list>
#include <set>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "graphics/Renderer.h"
#include "scene/SceneManager.h"
#include "files/FileSystem.h"
#include "input/Input.h"
#include "events/EventDispatcher.h"
#include "core/UpdateCallback.h"
#include "core/Settings.h"

void ouzelMain(const std::vector<std::string>& args);

namespace ouzel
{
    class Engine: public Noncopyable
    {
    public:
        Engine();
        virtual ~Engine();

        static std::set<graphics::Renderer::Driver> getAvailableRenderDrivers();
        static std::set<audio::Audio::Driver> getAvailableAudioDrivers();

        bool init(Settings& newSettings, const std::function<void(void)>& beginCallback);
        const Settings& getSettings() const { return settings; }

        const EventDispatcherPtr& getEventDispatcher() const { return eventDispatcher; }
        const CachePtr& getCache() const { return cache; }
        const WindowPtr& getWindow() const { return window; }
        const graphics::RendererPtr& getRenderer() const { return renderer; }
        const audio::AudioPtr& getAudio() const { return audio; }
        const scene::SceneManagerPtr& getSceneManager() const { return sceneManager; }
        const FileSystemPtr& getFileSystem() const { return fileSystem; }
        const input::InputPtr& getInput() const { return input; }
        const LocalizationPtr& getLocalization() const { return localization; }

        void exit();

        void begin();
        void end();
        void pause();
        void resume();
        bool draw();
        bool isRunning() const { return running; }
        bool isActive() const { return active; }

        float getFPS() const { return currentFPS; }
        float getAccumulatedFPS() const { return accumulatedFPS; }

        void scheduleUpdate(const UpdateCallback& callback);
        void unscheduleUpdate(const UpdateCallback& callback);

    protected:
        void run(const std::function<void(void)>& beginCallback);

        Settings settings;

        EventDispatcherPtr eventDispatcher;
        input::InputPtr input;
        WindowPtr window;
        FileSystemPtr fileSystem;
        LocalizationPtr localization;
        graphics::RendererPtr renderer;
        audio::AudioPtr audio;
        CachePtr cache;
        scene::SceneManagerPtr sceneManager;

        std::atomic<float> currentFPS;
        uint64_t previousFrameTime;

        float accumulatedTime = 0.0f;
        float currentAccumulatedFPS = 0.0f;
        std::atomic<float> accumulatedFPS;

        uint64_t previousUpdateTime;

        std::list<const UpdateCallback*> updateCallbacks;
        std::list<const UpdateCallback*>::iterator updateCallbackIterator;
        bool updateCallbackDeleted = false;
        std::thread updateThread;

        std::atomic<bool> running;
        std::atomic<bool> active;
    };

    extern Engine* sharedEngine;
}
