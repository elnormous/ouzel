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
#include "core/Application.hpp"
#include "core/Timer.hpp"
#include "core/Window.hpp"
#include "graphics/Renderer.hpp"
#include "audio/Audio.hpp"
#include "events/EventDispatcher.hpp"
#include "input/InputManager.hpp"
#include "scene/Scene.hpp"
#include "scene/SceneManager.hpp"
#include "storage/FileSystem.hpp"
#include "assets/Bundle.hpp"
#include "assets/Cache.hpp"
#include "assets/Loader.hpp"
#include "localization/Localization.hpp"
#include "network/Network.hpp"
#include "utils/Ini.hpp"
#include "utils/Log.hpp"
#include "utils/Thread.hpp"

namespace ouzel
{
    std::unique_ptr<Application> main(const std::vector<std::string>& args);

    const std::string SHADER_TEXTURE = "shaderTexture";
    const std::string SHADER_COLOR = "shaderColor";

    const std::string BLEND_NO_BLEND = "blendNoBlend";
    const std::string BLEND_ADD = "blendAdd";
    const std::string BLEND_MULTIPLY = "blendMultiply";
    const std::string BLEND_ALPHA = "blendAlpha";
    const std::string BLEND_SCREEN = "blendScreen";

    const std::string TEXTURE_WHITE_PIXEL = "textureWhitePixel";

    class Engine
    {
    public:
        Engine();
        virtual ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        Engine(Engine&&) = delete;
        Engine& operator=(Engine&&) = delete;

        void init();

        inline auto& getArgs() const noexcept { return args; }

        inline Log log(Log::Level level = Log::Level::Info) const { return logger.log(level); }
        inline auto& getLogger() { return logger; }
        inline auto& getLogger() const { return logger; }

        inline auto& getFileSystem() { return fileSystem; }
        inline auto& getFileSystem() const { return fileSystem; }

        inline auto& getEventDispatcher() { return eventDispatcher; }
        inline auto& getEventDispatcher() const { return eventDispatcher; }

        inline auto& getCache() { return cache; }
        inline auto& getCache() const { return cache; }

        inline auto getWindow() const { return window.get(); }

        inline auto getRenderer() const noexcept { return renderer.get(); }
        inline auto getAudio() const noexcept { return audio.get(); }

        inline auto& getSceneManager() { return sceneManager; }
        inline auto& getSceneManager() const { return sceneManager; }

        inline auto getInputManager() const noexcept { return inputManager.get(); }

        inline auto& getLocalization() { return localization; }
        inline auto& getLocalization() const { return localization; }

        inline auto& getNetwork() { return network; }
        inline auto& getNetwork() const { return network; }

        inline auto& getDefaultSettings() const noexcept { return defaultSettings; }
        inline auto& getUserSettings() const noexcept { return userSettings; }

        void start();
        void pause();
        void resume();
        void exit();

        inline bool isPaused() const noexcept { return paused; }
        inline bool isActive() const noexcept { return active; }

        void update();

        void executeOnMainThread(const std::function<void()>& func);

        virtual void openUrl(const std::string& url);

        inline bool isScreenSaverEnabled() const noexcept { return screenSaverEnabled; }
        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled);

        inline bool isOneUpdatePerFrame() const noexcept { return oneUpdatePerFrame; }
        inline void setOneUpdatePerFrame(bool value) { oneUpdatePerFrame = value; }

    protected:
        class Command final
        {
        public:
            enum class Type
            {
                Execute,
                OpenUrl,
                SetScreensaverEnabled
            };

            std::function<void()> func;
            std::string url;
            bool enabled = false;
        };

        virtual void engineMain();
        virtual void runOnMainThread(const std::function<void()>& func) = 0;

        Logger logger;
        storage::FileSystem fileSystem;
        EventDispatcher eventDispatcher;
        std::unique_ptr<Window> window;
        std::unique_ptr<graphics::Renderer> renderer;
        std::unique_ptr<audio::Audio> audio;
        std::unique_ptr<input::InputManager> inputManager;
        Localization localization;
        assets::Cache cache;
        assets::Bundle assetBundle;
        scene::SceneManager sceneManager;
        network::Network network;

        ini::Data defaultSettings;
        ini::Data userSettings;

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
        std::vector<std::string> args;
    };

    extern Engine* engine;
}

#endif // OUZEL_CORE_ENGINE_HPP
