// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>
#include "input/Input.hpp"
#include "input/InputSystem.hpp"
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

            inline const Vector2& getCursorPosition() const { return cursorPosition; }
            virtual void setCursorPosition(const Vector2& position);

            virtual void startDeviceDiscovery();
            virtual void stopDeviceDiscovery();

            bool isKeyboardKeyDown(KeyboardKey key) const { return keyboardKeyStates[static_cast<uint32_t>(key)]; }
            bool isMouseButtonDown(MouseButton button) const { return mouseButtonStates[static_cast<uint32_t>(button)]; }

            virtual void keyPress(KeyboardKey key, uint32_t modifiers);
            virtual void keyRelease(KeyboardKey key, uint32_t modifiers);

            virtual void mouseButtonPress(MouseButton button, const Vector2& position, uint32_t modifiers);
            virtual void mouseButtonRelease(MouseButton button, const Vector2& position, uint32_t modifiers);
            virtual void mouseMove(const Vector2& position, uint32_t modifiers);
            virtual void mouseRelativeMove(const Vector2& relativePosition, uint32_t modifiers);
            virtual void mouseScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers);

            virtual void touchBegin(uint64_t touchId, const Vector2& position, float force = 1.0F);
            virtual void touchEnd(uint64_t touchId, const Vector2& position, float force = 1.0F);
            virtual void touchMove(uint64_t touchId, const Vector2& position, float force = 1.0F);
            virtual void touchCancel(uint64_t touchId, const Vector2& position, float force = 1.0F);

            virtual void showVirtualKeyboard();
            virtual void hideVirtualKeyboard();

        protected:
            InputManager();

            void setCurrentCursor(Cursor* cursor);
            virtual void activateNativeCursor(NativeCursor* resource);
            virtual NativeCursor* createNativeCursor();
            void deleteNativeCursor(NativeCursor* resource);

            Vector2 cursorPosition;
            bool keyboardKeyStates[static_cast<uint32_t>(KeyboardKey::KEY_COUNT)];
            bool mouseButtonStates[static_cast<uint32_t>(MouseButton::BUTTON_COUNT)];

            std::unordered_map<uint64_t, Vector2> touchPositions;
            std::vector<std::unique_ptr<Gamepad>> gamepads;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<NativeCursor>> resources;
            std::vector<std::unique_ptr<NativeCursor>> resourceDeleteSet;
            NativeCursor* currentNativeCursor = nullptr;

            std::unordered_map<InputDevice*, Input> inputs;
        };
    } // namespace input
} // namespace ouzel
