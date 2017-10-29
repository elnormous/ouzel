// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <set>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include "Setup.h"
#include "utils/Noncopyable.hpp"
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
#include "utils/INI.hpp"

void ouzelMain(const std::vector<std::string>& args);

namespace ouzel
{
    class Window;

    class Engine: public Noncopyable
    {
    public:
        Engine();
        virtual ~Engine();

        bool init();

        inline const std::vector<std::string>& getArgs() { return args; }

        inline FileSystem* getFileSystem() { return fileSystem.get(); }
        inline EventDispatcher* getEventDispatcher() { return &eventDispatcher; }
        inline Timer* getTimer() { return &timer; }
        inline assets::Cache* getCache() { return &cache; }
        inline Window* getWindow() { return &window; }
        inline graphics::Renderer* getRenderer() const { return renderer.get(); }
        inline audio::Audio* getAudio() const { return audio.get(); }
        inline scene::SceneManager* getSceneManager() { return &sceneManager; }
        inline input::Input* getInput() const { return input.get(); }
        inline Localization* getLocalization() { return &localization; }
        inline network::Network* getNetwork() { return &network; }

        inline const INI& getDefaultSettings() const { return defaultSettings; }
        inline const INI& getUserSettings() const { return userSettings; }

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

        virtual bool setCurrentThreadName(const std::string& name);

    protected:
        virtual void main();
        void executeAll();

        std::unique_ptr<FileSystem> fileSystem;
        EventDispatcher eventDispatcher;
        Timer timer;
        Window window;
        std::unique_ptr<graphics::Renderer> renderer;
        std::unique_ptr<audio::Audio> audio;
        std::unique_ptr<input::Input> input;
        Localization localization;
        assets::Cache cache;
        scene::SceneManager sceneManager;
        network::Network network;

        INI defaultSettings;
        INI userSettings;

        std::chrono::steady_clock::time_point previousUpdateTime;

        std::vector<UpdateCallback*> updateCallbacks;
        std::set<UpdateCallback*> updateCallbackAddSet;
        std::set<UpdateCallback*> updateCallbackDeleteSet;

#if OUZEL_MULTITHREADED
        std::thread updateThread;
        std::mutex updateMutex;
        std::condition_variable updateCondition;
#endif

        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;

        std::atomic<bool> active;
        std::atomic<bool> paused;

        std::atomic<bool> screenSaverEnabled;
        std::vector<std::string> args;
    };

    extern Engine* engine;
}
