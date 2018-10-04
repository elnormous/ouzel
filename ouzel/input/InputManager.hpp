// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>
#include "input/InputSystem.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    class Engine;

    namespace input
    {
        class Controller;
        class Cursor;
        class NativeCursor;
        class Keyboard;
        class Gamepad;
        class Mouse;
        class Touchpad;

        class InputManager
        {
            friend Engine;
            friend Cursor;
            friend NativeCursor;
        public:
            virtual ~InputManager();

            InputManager(const InputManager&) = delete;
            InputManager& operator=(const InputManager&) = delete;

            InputManager(InputManager&&) = delete;
            InputManager& operator=(InputManager&&) = delete;

            InputSystem* getInputSystem() const { return inputSystem.get(); }

            const std::vector<Controller*>& getControllers() const { return controllers; }
            Keyboard* getKeyboard() const { return keyboard; }
            Mouse* getMouse() const { return mouse; }
            Touchpad* getTouchpad() const { return touchpad; }

            void update();

            template<typename T>
            void setCursor(const std::unique_ptr<T>& cursor)
            {
                setCurrentCursor(cursor.get());
            }

            void setCursor(Cursor* cursor)
            {
                setCurrentCursor(cursor);
            }

            virtual void setCursorVisible(bool visible);
            virtual bool isCursorVisible() const;

            virtual void setCursorLocked(bool locked);
            virtual bool isCursorLocked() const;

            void startDeviceDiscovery();
            void stopDeviceDiscovery();

            void showVirtualKeyboard();
            void hideVirtualKeyboard();

        protected:
            InputManager();

            void setCurrentCursor(Cursor* cursor);
            virtual void activateNativeCursor(NativeCursor* resource);
            virtual NativeCursor* createNativeCursor();
            void deleteNativeCursor(NativeCursor* resource);

            std::unique_ptr<InputSystem> inputSystem;
            Keyboard* keyboard;
            Mouse* mouse;
            Touchpad* touchpad;

            std::vector<std::unique_ptr<Gamepad>> gamepads;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<NativeCursor>> resources;
            std::vector<std::unique_ptr<NativeCursor>> resourceDeleteSet;
            NativeCursor* currentNativeCursor = nullptr;

            std::unordered_map<uint32_t, std::unique_ptr<Controller>> controllerMap;
            std::vector<Controller*> controllers;

            bool discovering = false;
        };
    } // namespace input
} // namespace ouzel
