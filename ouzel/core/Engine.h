// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <set>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include "CompileConfig.h"
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "core/UpdateCallback.h"
#include "core/Settings.h"
#include "events/EventDispatcher.h"
#include "scene/SceneManager.h"
#include "core/Cache.h"
#include "localization/Localization.h"

void ouzelMain(const std::vector<std::string>& args);

namespace ouzel
{
    class Window;
    class EventDispatcher;
    class Cache;
    
    class Engine: public Noncopyable
    {
    public:
        Engine();
        ~Engine();

        static std::set<graphics::Renderer::Driver> getAvailableRenderDrivers();
        static std::set<audio::Audio::Driver> getAvailableAudioDrivers();

        bool init(Settings& newSettings);
        const Settings& getSettings() const { return settings; }

        EventDispatcher* getEventDispatcher() { return &eventDispatcher; }
        Cache* getCache() { return &cache; }
        Window* getWindow() const { return window.get(); }
        graphics::Renderer* getRenderer() const { return renderer.get(); }
        audio::Audio* getAudio() const { return audio.get(); }
        scene::SceneManager* getSceneManager() { return &sceneManager; }
        input::Input* getInput() const { return input.get(); }
        Localization* getLocalization() { return &localization; }

        void exit();
        void pause();
        void resume();
        bool draw();
        bool isRunning() const { return running; }
        bool isActive() const { return active; }

        float getFPS() const { return currentFPS; }
        float getAccumulatedFPS() const { return accumulatedFPS; }

        void scheduleUpdate(const UpdateCallback* callback);
        void unscheduleUpdate(const UpdateCallback* callback);

        void exitUpdateThread();

    protected:
        void run();

        Settings settings;

        EventDispatcher eventDispatcher;
        Localization localization;
        Cache cache;
        std::unique_ptr<Window> window;
        std::unique_ptr<graphics::Renderer> renderer;
        std::unique_ptr<audio::Audio> audio;
        std::unique_ptr<input::Input> input;
        scene::SceneManager sceneManager;

        std::atomic<float> currentFPS;
        std::chrono::steady_clock::time_point previousFrameTime;

        float accumulatedTime = 0.0f;
        float currentAccumulatedFPS = 0.0f;
        std::atomic<float> accumulatedFPS;

        std::chrono::steady_clock::time_point previousUpdateTime;

        std::vector<const UpdateCallback*> updateCallbacks;
        std::set<const UpdateCallback*> updateCallbackAddSet;

#if OUZEL_MULTITHREADED
        std::thread updateThread;
#endif

        std::atomic<bool> running;
        std::atomic<bool> active;
    };

    extern Engine* sharedEngine;
}
