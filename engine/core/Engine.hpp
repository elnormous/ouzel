// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_ENGINE_HPP
#define OUZEL_CORE_ENGINE_HPP

#include <atomic>
#include <chrono>
#include <functional>
#include <random>
#include <thread>
#include <vector>
#include "Application.hpp"
#include "Timer.hpp"
#include "Window.hpp"
#include "WorkerPool.hpp"
#include "../graphics/Graphics.hpp"
#include "../audio/Audio.hpp"
#include "../events/EventDispatcher.hpp"
#include "../input/InputManager.hpp"
#include "../scene/SceneManager.hpp"
#include "../storage/FileSystem.hpp"
#include "../assets/Bundle.hpp"
#include "../assets/Cache.hpp"
#include "../localization/Localization.hpp"
#include "../network/Network.hpp"
#include "../formats/Ini.hpp"
#include "../utils/Log.hpp"
#include "../thread/Thread.hpp"

namespace ouzel::core
{
    inline std::mt19937 randomEngine{std::random_device{}()};

    struct Settings final
    {
        math::Size<std::uint32_t, 2> size{};
        graphics::Driver graphicsDriver;
        graphics::Settings graphicsSettings;
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true; // should high DPI resolution be used
        audio::Driver audioDriver;
        audio::Settings audioSettings;
    };

    class Engine
    {
    public:
        Engine(const std::vector<std::string>& initArgs);
        virtual ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        Engine(Engine&&) = delete;
        Engine& operator=(Engine&&) = delete;

        [[nodiscard]] auto& getFileSystem() noexcept { return fileSystem; }
        [[nodiscard]] auto& getFileSystem() const noexcept { return fileSystem; }

        [[nodiscard]] auto& getEventDispatcher() noexcept { return eventDispatcher; }
        [[nodiscard]] auto& getEventDispatcher() const noexcept { return eventDispatcher; }

        [[nodiscard]] auto& getCache() noexcept { return cache; }
        [[nodiscard]] auto& getCache() const noexcept { return cache; }

        [[nodiscard]] auto& getWindow() noexcept { return window; }
        [[nodiscard]] auto& getWindow() const noexcept { return window; }

        [[nodiscard]] auto& getGraphics() noexcept { return graphics; }
        [[nodiscard]] auto& getGraphics() const noexcept { return graphics; }

        [[nodiscard]] auto& getAudio() noexcept { return audio; }
        [[nodiscard]] auto& getAudio() const noexcept { return audio; }

        [[nodiscard]] auto& getSceneManager() noexcept { return sceneManager; }
        [[nodiscard]] auto& getSceneManager() const noexcept { return sceneManager; }

        [[nodiscard]] auto& getInputManager() noexcept { return inputManager; }
        [[nodiscard]] auto& getInputManager() const noexcept { return inputManager; }

        [[nodiscard]] auto& getLocalization() noexcept { return localization; }
        [[nodiscard]] auto& getLocalization() const noexcept { return localization; }

        [[nodiscard]] auto& getNetwork() noexcept { return network; }
        [[nodiscard]] auto& getNetwork() const noexcept { return network; }

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
        void setOneUpdatePerFrame(bool value) noexcept { oneUpdatePerFrame = value; }

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
        Settings settings;
        EventDispatcher eventDispatcher;
        Window window;
        graphics::Graphics graphics;
        audio::Audio audio;
        input::InputManager inputManager;
        Localization localization;
        assets::Cache cache;
        assets::Bundle assetBundle;
        scene::SceneManager sceneManager;
        network::Network network;
        WorkerPool workerPool;

    private:
        void handleEvents(std::queue<std::unique_ptr<Event>> windowEvents);
        std::vector<std::string> args;

#ifndef __EMSCRIPTEN__
        thread::Thread updateThread;
#endif
        std::chrono::steady_clock::time_point previousUpdateTime;

        std::atomic_bool active{false};
        std::atomic_bool paused{false};
        std::atomic_bool oneUpdatePerFrame{false};
        bool refillRenderQueue = true;

        std::atomic_bool screenSaverEnabled{true};
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
