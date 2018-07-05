// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include "math/Vector2.hpp"
#include "thread/Mutex.hpp"

namespace ouzel
{
    class Engine;

    namespace input
    {
        class Cursor;
        class NativeCursor;

        enum class KeyboardKey
        {
            NONE,
            CANCEL,         // Control-break processing
            BACKSPACE,      // BACKSPACE key
            TAB,            // TAB key
            CLEAR,          // CLEAR key
            RETURN,         // ENTER key
            PAUSE,          // PAUSE key
            CAPITAL,        // CAPS LOCK key
            KANA,           // IME Kana mode
            HANGUEL,        // IME Hanguel mode (maintained for compatibility use HANGUL)
            HANGUL,         // IME Hangul mode
            JUNJA,          // IME Junja mode
            FINAL,          // IME final mode
            HANJA,          // IME Hanja mode
            KANJI,          // IME Kanji mode
            ESCAPE,         // ESC key
            CONVERT,        // IME convert
            NONCONVERT,     // IME nonconvert
            ACCEPT,         // IME accept
            MODECHANGE,     // IME mode change request
            SPACE,          // SPACEBAR
            PRIOR,          // PAGE UP key
            NEXT,           // PAGE DOWN key
            END,            // END key
            HOME,           // HOME key
            LEFT,           // LEFT ARROW key
            UP,             // UP ARROW key
            RIGHT,          // RIGHT ARROW key
            DOWN,           // DOWN ARROW key
            SELECT,         // SELECT key
            PRINT,          // PRINT key
            EXECUTE,        // EXECUTE key
            SNAPSHOT,       // PRINT SCREEN key
            INSERT,         // INS key
            DEL,            // DEL key
            EQUAL,          // EQUAL key
            HELP,           // HELP key
            NUM_0,          // 0 key
            NUM_1,          // 1 key
            NUM_2,          // 2 key
            NUM_3,          // 3 key
            NUM_4,          // 4 key
            NUM_5,          // 5 key
            NUM_6,          // 6 key
            NUM_7,          // 7 key
            NUM_8,          // 8 key
            NUM_9,          // 9 key
            A,              // A key
            B,              // B key
            C,              // C key
            D,              // D key
            E,              // E key
            F,              // F key
            G,              // G key
            H,              // H key
            I,              // I key
            J,              // J key
            K,              // K key
            L,              // L key
            M,              // M key
            N,              // N key
            O,              // O key
            P,              // P key
            Q,              // Q key
            R,              // R key
            S,              // S key
            T,              // T key
            U,              // U key
            V,              // V key
            W,              // W key
            X,              // X key
            Y,              // Y key
            Z,              // Z key
            LEFT_SUPER,     // Left Windows key (Microsoft Natural keyboard) on Windows, left Command key on macOS
            RIGHT_SUPER,    // Right Windows key (Natural keyboard) on Windows, right Command key on macOS
            MENU,           // Applications key (Natural keyboard)
            SLEEP,          // Computer Sleep key
            NUMPAD_0,       // Numeric keypad 0 key
            NUMPAD_1,       // Numeric keypad 1 key
            NUMPAD_2,       // Numeric keypad 2 key
            NUMPAD_3,       // Numeric keypad 3 key
            NUMPAD_4,       // Numeric keypad 4 key
            NUMPAD_5,       // Numeric keypad 5 key
            NUMPAD_6,       // Numeric keypad 6 key
            NUMPAD_7,       // Numeric keypad 7 key
            NUMPAD_8,       // Numeric keypad 8 key
            NUMPAD_9,       // Numeric keypad 9 key
            MULTIPLY,       // Multiply key
            ADD,            // Add key
            SEPARATOR,      // Separator key
            SUBTRACT,       // Subtract key
            DECIMAL,        // Decimal key
            DIVIDE,         // Divide key
            F1,             // F1 key
            F2,             // F2 key
            F3,             // F3 key
            F4,             // F4 key
            F5,             // F5 key
            F6,             // F6 key
            F7,             // F7 key
            F8,             // F8 key
            F9,             // F9 key
            F10,            // F10 key
            F11,            // F11 key
            F12,            // F12 key
            F13,            // F13 key
            F14,            // F14 key
            F15,            // F15 key
            F16,            // F16 key
            F17,            // F17 key
            F18,            // F18 key
            F19,            // F19 key
            F20,            // F20 key
            F21,            // F21 key
            F22,            // F22 key
            F23,            // F23 key
            F24,            // F24 key
            NUMLOCK,        // NUM LOCK key
            SCROLL,         // SCROLL LOCK key
            LEFT_SHIFT,     // Left SHIFT key
            RIGHT_SHIFT,    // Right SHIFT key
            LEFT_CONTROL,   // Left CONTROL key
            RIGHT_CONTROL,  // Right CONTROL key
            LEFT_ALT,       // Left alt key
            RIGHT_ALT,      // Right alt key
            SEMICOLON,      // for US    ";:"
            PLUS,           // Plus Key   "+"
            COMMA,          // Comma Key  ","
            MINUS,          // Minus Key  "-"
            PERIOD,         // Period Key "."
            SLASH,          // for US    "/?"
            GRAVE,          // for US    "`~"
            LEFT_BRACKET,   // for US    "[{"
            BACKSLASH,      // for US    "\|"
            RIGHT_BRACKET,  // for US    "]}"
            QUOTE,          // for US    "'""
            OEM_AX,         // for Japan "AX"
            LESS,           // "<>" or "\|"
            ATTN,           // Attn key
            CRSEL,          // CrSel key
            EXSEL,          // ExSel key
            EREOF,          // Erase EOF key
            PLAY,           // Play key
            ZOOM,           // Zoom key
            PA1,            // PA1 key
            OEM_CLEAR,      // Clear key

            KEY_COUNT
        };

        enum class MouseButton
        {
            NONE,
            LEFT,           // Left mouse button
            RIGHT,          // Right mouse button
            MIDDLE,         // Middle mouse button (three-button mouse)
            X1,             // Windows 2000/XP: X1 mouse button
            X2,              // Windows 2000/XP: X2 mouse button

            BUTTON_COUNT
        };

        enum class GamepadButton
        {
            NONE,
            DPAD_LEFT,
            DPAD_RIGHT,
            DPAD_UP,
            DPAD_DOWN,
            FACE_BOTTOM, // A on Xbox controller, cross on PS controller
            FACE_RIGHT, // B on Xbox controller, circle on PS controller
            FACE_LEFT, // X on Xbox controller, square on PS controller
            FACE_TOP, // Y on Xbox controller, triangle on PS controller
            LEFT_SHOULDER,  // L1 for Apple and PS controller
            LEFT_TRIGGER,   // L2 for Apple and PS controller
            RIGHT_SHOULDER, // R1 for Apple and PS controller
            RIGHT_TRIGGER,  // R2 for Apple and PS controller
            LEFT_THUMB,
            RIGHT_THUMB,
            START,
            BACK,
            PAUSE,
            LEFT_THUMB_LEFT,
            LEFT_THUMB_RIGHT,
            LEFT_THUMB_UP,
            LEFT_THUMB_DOWN,
            RIGHT_THUMB_LEFT,
            RIGHT_THUMB_RIGHT,
            RIGHT_THUMB_UP,
            RIGHT_THUMB_DOWN,

            BUTTON_COUNT
        };

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

            virtual void startGamepadDiscovery();
            virtual void stopGamepadDiscovery();

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

            Mutex resourceMutex;
            std::vector<std::unique_ptr<NativeCursor>> resources;
            std::vector<std::unique_ptr<NativeCursor>> resourceDeleteSet;
            NativeCursor* currentNativeCursor = nullptr;
        };
    } // namespace input
} // namespace ouzel
