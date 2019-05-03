// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
#include "localization/Localization.hpp"
#include "network/Network.hpp"
#include "utils/Ini.hpp"
#include "utils/Log.hpp"

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

        inline const std::vector<std::string>& getArgs() const { return args; }

        inline Log log(Log::Level level = Log::Level::INFO) const { return logger.log(level); }
        inline Logger& getLogger() { return logger; }

        inline storage::FileSystem& getFileSystem() { return fileSystem; }
        inline EventDispatcher& getEventDispatcher() { return eventDispatcher; }
        inline assets::Cache& getCache() { return cache; }
        inline Window* getWindow() { return window.get(); }
        inline graphics::Renderer* getRenderer() const { return renderer.get(); }
        inline audio::Audio* getAudio() const { return audio.get(); }
        inline scene::SceneManager& getSceneManager() { return sceneManager; }
        inline input::InputManager* getInputManager() const { return inputManager.get(); }
        inline Localization& getLocalization() { return localization; }
        inline network::Network& getNetwork() { return network; }

        inline const ini::Data& getDefaultSettings() const { return defaultSettings; }
        inline const ini::Data& getUserSettings() const { return userSettings; }

        void start();
        void pause();
        void resume();
        void exit();

        inline bool isPaused() const { return paused; }
        inline bool isActive() const { return active; }

        void update();

        void executeOnMainThread(const std::function<void()>& func);

        virtual void openURL(const std::string& url);

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled);
        inline bool isScreenSaverEnabled() const { return screenSaverEnabled; }

        inline bool isOneUpdatePerFrame() const { return oneUpdatePerFrame; }
        inline void setOneUpdatePerFrame(bool value) { oneUpdatePerFrame = value; }

    protected:
        class Command final
        {
        public:
            enum class Type
            {
                EXECUTE,
                OPEN_URL,
                SET_SCREENSAVER_ENABLED
            };

            std::function<void()> func;
            std::string url;
            bool enabled = false;
        };

        virtual void main();
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
        std::thread updateThread;
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
