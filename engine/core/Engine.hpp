// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINE_HPP
#define OUZEL_CORE_ENGINE_HPP

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include "Application.hpp"
#include "Timer.hpp"
#include "Window.hpp"
#include "../graphics/Graphics.hpp"
#include "../audio/Audio.hpp"
#include "../events/EventDispatcher.hpp"
#include "../input/InputManager.hpp"
#include "../scene/Scene.hpp"
#include "../scene/SceneManager.hpp"
#include "../storage/FileSystem.hpp"
#include "../assets/Bundle.hpp"
#include "../assets/Cache.hpp"
#include "../assets/Loader.hpp"
#include "../localization/Localization.hpp"
#include "../network/Network.hpp"
#include "../formats/Ini.hpp"
#include "../utils/Log.hpp"
#include "../utils/Thread.hpp"

namespace ouzel::core
{
    class Engine
    {
    public:
        Engine(const std::vector<std::string>& initArgs);
        virtual ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        Engine(Engine&&) = delete;
        Engine& operator=(Engine&&) = delete;

        void init();

        auto& getFileSystem() { return fileSystem; }
        auto& getFileSystem() const { return fileSystem; }

        auto& getEventDispatcher() { return eventDispatcher; }
        auto& getEventDispatcher() const { return eventDispatcher; }

        auto& getCache() { return cache; }
        auto& getCache() const { return cache; }

        auto getWindow() const { return window.get(); }

        auto getGraphics() const noexcept { return graphics.get(); }
        auto getAudio() const noexcept { return audio.get(); }

        auto& getSceneManager() { return sceneManager; }
        auto& getSceneManager() const { return sceneManager; }

        auto getInputManager() const noexcept { return inputManager.get(); }

        auto& getLocalization() { return localization; }
        auto& getLocalization() const { return localization; }

        auto& getNetwork() { return network; }
        auto& getNetwork() const { return network; }

        void start();
        void pause();
        void resume();
        void exit();

        bool isPaused() const noexcept { return paused; }
        bool isActive() const noexcept { return active; }

        void update();

        void executeOnMainThread(const std::function<void()>& func);

        virtual void openUrl(const std::string& url);

        bool isScreenSaverEnabled() const noexcept { return screenSaverEnabled; }
        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled);

        bool isOneUpdatePerFrame() const noexcept { return oneUpdatePerFrame; }
        void setOneUpdatePerFrame(bool value) { oneUpdatePerFrame = value; }

    protected:
        class Command final
        {
        public:
            enum class Type
            {
                execute,
                openUrl,
                setScreensaverEnabled
            };

            std::function<void()> func;
            std::string url;
            bool enabled = false;
        };

        virtual void engineMain();
        virtual void runOnMainThread(const std::function<void()>& func) = 0;

        storage::FileSystem fileSystem;
        EventDispatcher eventDispatcher;
        std::unique_ptr<Window> window;
        std::unique_ptr<graphics::Graphics> graphics;
        std::unique_ptr<audio::Audio> audio;
        std::unique_ptr<input::InputManager> inputManager;
        Localization localization;
        assets::Cache cache;
        assets::Bundle assetBundle;
        scene::SceneManager sceneManager;
        network::Network network;

#if !defined(__EMSCRIPTEN__)
        Thread updateThread;
        std::mutex updateMutex;
        std::condition_variable updateCondition;
#endif
        std::chrono::steady_clock::time_point previousUpdateTime;

        std::atomic_bool active{false};
        std::atomic_bool paused{false};
        std::atomic_bool oneUpdatePerFrame{false};

        std::atomic_bool screenSaverEnabled{true};

    private:
        std::vector<std::string> args;
    };
}

namespace ouzel
{
    const std::string shaderTexture = "shaderTexture";
    const std::string shaderColor = "shaderColor";

    const std::string blendNoBlend = "blendNoBlend";
    const std::string blendAdd = "blendAdd";
    const std::string blendMultiply = "blendMultiply";
    const std::string blendAlpha = "blendAlpha";
    const std::string blendScreen = "blendScreen";

    const std::string textureWhitePixel = "textureWhitePixel";

    std::unique_ptr<Application> main(const std::vector<std::string>& args);
    extern core::Engine* engine;
    extern Logger logger;
}

#endif // OUZEL_CORE_ENGINE_HPP
