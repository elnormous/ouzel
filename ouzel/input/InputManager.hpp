// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTMANAGER_HPP
#define OUZEL_INPUT_INPUTMANAGER_HPP

#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include <unordered_map>
#include "input/InputSystem.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    namespace input
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
            inline InputSystem* getInputSystem() const { return inputSystem.get(); }

            inline const std::vector<Controller*>& getControllers() const { return controllers; }
            inline Keyboard* getKeyboard() const { return keyboard; }
            inline Mouse* getMouse() const { return mouse; }
            inline Touchpad* getTouchpad() const { return touchpad; }

            void startDeviceDiscovery();
            void stopDeviceDiscovery();

            void showVirtualKeyboard();
            void hideVirtualKeyboard();

        private:
            std::future<bool> eventCallback(const InputSystem::Event& event);
            bool handleEvent(const InputSystem::Event& event);

            std::mutex eventQueueMutex;
            std::queue<std::pair<std::promise<bool>, InputSystem::Event>> eventQueue;

            std::unique_ptr<InputSystem> inputSystem;
            Keyboard* keyboard = nullptr;
            Mouse* mouse = nullptr;
            Touchpad* touchpad = nullptr;

            std::unordered_map<uint32_t, std::unique_ptr<Controller>> controllerMap;
            std::vector<Controller*> controllers;

            bool discovering = false;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_INPUTMANAGER_HPP
