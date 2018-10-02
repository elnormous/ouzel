// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>
#include "input/Controller.hpp"
#include "input/InputSystem.hpp"
#include "input/Gamepad.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    class Engine;

    namespace input
    {
        class Cursor;
        class NativeCursor;
        class Gamepad;

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

            std::vector<std::unique_ptr<Gamepad>> gamepads;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<NativeCursor>> resources;
            std::vector<std::unique_ptr<NativeCursor>> resourceDeleteSet;
            NativeCursor* currentNativeCursor = nullptr;

            std::unordered_map<uint32_t, std::unique_ptr<Controller>> controllers;

            bool discovering = false;
        };
    } // namespace input
} // namespace ouzel
