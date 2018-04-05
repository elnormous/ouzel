// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
#include <vector>
#include <functional>
#include <atomic>
#include <chrono>
#include "Setup.h"
#include "core/UpdateCallback.hpp"
#include "core/Timer.hpp"
#include "core/Window.hpp"
#include "graphics/Renderer.hpp"
#include "audio/Audio.hpp"
#include "files/FileSystem.hpp"
#include "events/EventDispatcher.hpp"
#include "scene/SceneManager.hpp"
#include "assets/Cache.hpp"
#include "localization/Localization.hpp"
#include "network/Network.hpp"
#include "thread/Condition.hpp"
#include "thread/Mutex.hpp"
#include "thread/Thread.hpp"
#include "utils/INI.hpp"

void ouzelMain(const std::vector<std::string>& args);

namespace ouzel
{
    class Engine
    {
    public:
        Engine();
        virtual ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        Engine(Engine&&) = delete;
        Engine& operator=(Engine&&) = delete;

        bool init();

        inline const std::vector<std::string>& getArgs() const { return args; }

        inline FileSystem* getFileSystem() { return &fileSystem; }
        inline EventDispatcher* getEventDispatcher() { return &eventDispatcher; }
        inline assets::Cache* getCache() { return &cache; }
        inline Window* getWindow() { return &window; }
        inline graphics::Renderer* getRenderer() const { return renderer.get(); }
        inline audio::Audio* getAudio() const { return audio.get(); }
        inline scene::SceneManager* getSceneManager() { return &sceneManager; }
        inline input::Input* getInput() const { return input.get(); }
        inline Localization* getLocalization() { return &localization; }
        inline network::Network* getNetwork() { return &network; }

        inline const ini::Data& getDefaultSettings() const { return defaultSettings; }
        inline const ini::Data& getUserSettings() const { return userSettings; }

        virtual int run();
        void start();
        void pause();
        void resume();
        void exit();

        inline bool isPaused() const { return paused; }
        inline bool isActive() const { return active; }

        void scheduleUpdate(UpdateCallback* callback);
        void unscheduleUpdate(UpdateCallback* callback);

        void executeOnUpdateThread(const std::function<void(void)>& func);

        void update();

        virtual void executeOnMainThread(const std::function<void(void)>& func) = 0;

        virtual bool openURL(const std::string& url);

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled);
        bool isScreenSaverEnabled() const { return screenSaverEnabled; }

    protected:
        virtual void main();
        void executeAll();

        FileSystem fileSystem;
        EventDispatcher eventDispatcher;
        Window window;
        std::unique_ptr<graphics::Renderer> renderer;
        std::unique_ptr<audio::Audio> audio;
        std::unique_ptr<input::Input> input;
        Localization localization;
        assets::Cache cache;
        scene::SceneManager sceneManager;
        network::Network network;

        ini::Data defaultSettings;
        ini::Data userSettings;

        std::chrono::steady_clock::time_point previousUpdateTime;

        std::vector<UpdateCallback*> updateCallbacks;
        std::set<UpdateCallback*> updateCallbackAddSet;
        std::set<UpdateCallback*> updateCallbackDeleteSet;

#if OUZEL_MULTITHREADED
        Thread updateThread;
        Mutex updateMutex;
        Condition updateCondition;
#endif

        std::queue<std::function<void(void)>> updateThreadExecuteQueue;
        Mutex updateThreadExecuteMutex;

        std::atomic<bool> active;
        std::atomic<bool> paused;

        std::atomic<bool> screenSaverEnabled;
        std::vector<std::string> args;
    };

    extern Engine* engine;
}
