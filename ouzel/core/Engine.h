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

        bool init(Settings& newSettings);
        const Settings& getSettings() const { return settings; }

        EventDispatcher* getEventDispatcher() const { return eventDispatcher.get(); }
        Cache* getCache() const { return cache.get(); }
        Window* getWindow() const { return window.get(); }
        graphics::Renderer* getRenderer() const { return renderer.get(); }
        audio::Audio* getAudio() const { return audio.get(); }
        scene::SceneManager* getSceneManager() const { return sceneManager.get(); }
        input::Input* getInput() const { return input.get(); }
        Localization* getLocalization() const { return localization.get(); }

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
        void run();

        Settings settings;

        std::unique_ptr<EventDispatcher> eventDispatcher;
        std::unique_ptr<Window> window;
        std::unique_ptr<Localization> localization;
        std::unique_ptr<graphics::Renderer> renderer;
        std::unique_ptr<audio::Audio> audio;
        std::unique_ptr<input::Input> input;
        std::unique_ptr<Cache> cache;
        std::unique_ptr<scene::SceneManager> sceneManager;

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
