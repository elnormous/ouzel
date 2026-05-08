#ifndef FLUXION_CORE_ENGINE_HPP
#define FLUXION_CORE_ENGINE_HPP

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

namespace fluxion::core
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

        // Multiplier applied to the per-frame delta delivered to game-side
        // UpdateEvent listeners (animations, sprite playback, etc.).
        // Default 1.0 = real time. 0.0 freezes simulation (rendering stays
        // live so editor gizmos / panels remain interactive). 0.5 = half
        // speed, 2.0 = double, and so on.
        //
        // Only the user-visible logical clock is scaled: input polling,
        // audio mixing, and the host platform's frame pacing are
        // unaffected. Atomic so the editor (host thread) and the engine
        // update loop don't race on this value.
        void setTimeScale(float scale) noexcept
        {
            timeScale.store(scale, std::memory_order_release);
        }
        [[nodiscard]] float getTimeScale() const noexcept
        {
            return timeScale.load(std::memory_order_acquire);
        }

        // Request that the next `n` update ticks run at real-time delta
        // regardless of the current timeScale. Intended for the editor's
        // "Step one frame" button while paused: a pause is timeScale=0,
        // and a step is requestStep(1) that briefly bypasses the scale.
        // The counter saturates at INT_MAX; calling with n<=0 is a no-op.
        void requestStep(int n = 1) noexcept
        {
            if (n <= 0) return;
            pendingSteps.fetch_add(n, std::memory_order_acq_rel);
        }
        [[nodiscard]] int getPendingSteps() const noexcept
        {
            return pendingSteps.load(std::memory_order_acquire);
        }

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
        l10n::Localization localization;
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

        std::atomic<float> timeScale{1.0F};
        std::atomic<int> pendingSteps{0};
    };
}

namespace fluxion
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

#endif // FLUXION_CORE_ENGINE_HPP
