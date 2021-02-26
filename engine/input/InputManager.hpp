// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_INPUTMANAGER_HPP
#define OUZEL_INPUT_INPUTMANAGER_HPP

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include <unordered_map>
#include "../math/Vector.hpp"

#if TARGET_OS_IOS
#  include "ios/InputSystemIOS.hpp"
#elif TARGET_OS_TV
#  include "tvos/InputSystemTVOS.hpp"
#elif TARGET_OS_MAC
#  include "macos/InputSystemMacOS.hpp"
#elif defined(__ANDROID__)
#  include "android/InputSystemAndroid.hpp"
#elif defined(__linux__)
#  include "linux/InputSystemLinux.hpp"
#elif defined(_WIN32)
#  include "windows/InputSystemWin.hpp"
#elif defined(__EMSCRIPTEN__)
#  include "emscripten/InputSystemEm.hpp"
#else
#  include "InputSystem.hpp"
#endif

namespace ouzel::input
{
    class Controller;
    class Keyboard;
    class Gamepad;
    class Mouse;
    class Touchpad;

    class InputManager final
    {
    public:
        InputManager();

        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;
        InputManager(InputManager&&) = delete;
        InputManager& operator=(InputManager&&) = delete;

        void update();
        auto& getInputSystem() noexcept { return inputSystem; }

        auto& getControllers() const noexcept { return controllers; }
        auto getKeyboard() const noexcept { return keyboard; }
        auto getMouse() const noexcept { return mouse; }
        auto getTouchpad() const noexcept { return touchpad; }

        void startDeviceDiscovery();
        void stopDeviceDiscovery();

        void showVirtualKeyboard();
        void hideVirtualKeyboard();

    private:
        std::future<bool> eventCallback(const InputSystem::Event& event);
        bool handleEvent(const InputSystem::Event& event);

        std::mutex eventQueueMutex;
        std::queue<std::pair<std::promise<bool>, InputSystem::Event>> eventQueue;

#if TARGET_OS_IOS
        ios::InputSystem inputSystem;
#elif TARGET_OS_TV
        tvos::InputSystem inputSystem;
#elif TARGET_OS_MAC
        macos::InputSystem inputSystem;
#elif defined(__ANDROID__)
        android::InputSystem inputSystem;
#elif defined(__linux__)
        linux::InputSystem inputSystem;
#elif defined(_WIN32)
        windows::InputSystem inputSystem;
#elif defined(__EMSCRIPTEN__)
        emscripten::InputSystem inputSystem;
#else
        InputSystem inputSystem;
#endif

        Keyboard* keyboard = nullptr;
        Mouse* mouse = nullptr;
        Touchpad* touchpad = nullptr;

        std::unordered_map<DeviceId, std::unique_ptr<Controller>> controllerMap;
        std::vector<Controller*> controllers;

        bool discovering = false;
    };
}

#endif // OUZEL_INPUT_INPUTMANAGER_HPP
