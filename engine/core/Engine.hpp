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
#include "../thread/Thread.hpp"

namespace ouzel::core
{
#if defined(__ARM_NEON__)
#  if defined(__ANDROID__) && defined(__arm__)
    // NEON support must be checked at runtime on 32-bit Android
    extern const bool isSimdAvailable;
#  else
    constexpr auto isSimdAvailable = true;
#  endif
#elif defined(__SSE__)
    constexpr auto isSimdAvailable = true;
#else
    constexpr auto isSimdAvailable = false;
#endif

    extern std::mt19937 randomEngine;

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

        [[nodiscard]] auto& getFileSystem() { return fileSystem; }
        [[nodiscard]] auto& getFileSystem() const { return fileSystem; }

        [[nodiscard]] auto& getEventDispatcher() { return eventDispatcher; }
        [[nodiscard]] auto& getEventDispatcher() const { return eventDispatcher; }

        [[nodiscard]] auto& getCache() { return cache; }
        [[nodiscard]] auto& getCache() const { return cache; }

        [[nodiscard]] auto getWindow() const { return window.get(); }

        [[nodiscard]] auto getGraphics() const noexcept { return graphics.get(); }
        [[nodiscard]] auto getAudio() const noexcept { return audio.get(); }

        [[nodiscard]] auto& getSceneManager() { return sceneManager; }
        [[nodiscard]] auto& getSceneManager() const { return sceneManager; }

        [[nodiscard]] auto getInputManager() const noexcept { return inputManager.get(); }

        [[nodiscard]] auto& getLocalization() { return localization; }
        [[nodiscard]] auto& getLocalization() const { return localization; }

        [[nodiscard]] auto& getNetwork() { return network; }
        [[nodiscard]] auto& getNetwork() const { return network; }

        void start();
        void pause();
        void resume();
        void exit();

        [[nodiscard]] bool isPaused() const noexcept { return paused; }
        [[nodiscard]] bool isActive() const noexcept { return active; }

        void update();

        void executeOnMainThread(const std::function<void()>& func);

        virtual void openUrl(const std::string& url);

        [[nodiscard]] bool isScreenSaverEnabled() const noexcept { return screenSaverEnabled; }
        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled);

        [[nodiscard]] bool isOneUpdatePerFrame() const noexcept { return oneUpdatePerFrame; }
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
        thread::Thread updateThread;
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
    inline core::Engine* engine = nullptr;
}

#endif // OUZEL_CORE_ENGINE_HPP
