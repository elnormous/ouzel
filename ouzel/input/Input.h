// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <memory>
#include "utils/Noncopyable.h"
#include "utils/Types.h"
#include "math/Vector2.h"

namespace ouzel
{
    class Engine;
    namespace input
    {
        enum class KeyboardKey
        {
            NONE,
            CANCEL,         // Control-break processing
            BACKSPACE,      // BACKSPACE key
            TAB,            // TAB key
            CLEAR,          // CLEAR key
            RETURN,         // ENTER key
            SHIFT,          // SHIFT key
            CONTROL,        // CTRL key
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
            KEY_0,          // 0 key
            KEY_1,          // 1 key
            KEY_2,          // 2 key
            KEY_3,          // 3 key
            KEY_4,          // 4 key
            KEY_5,          // 5 key
            KEY_6,          // 6 key
            KEY_7,          // 7 key
            KEY_8,          // 8 key
            KEY_9,          // 9 key
            KEY_A,          // A key
            KEY_B,          // B key
            KEY_C,          // C key
            KEY_D,          // D key
            KEY_E,          // E key
            KEY_F,          // F key
            KEY_G,          // G key
            KEY_H,          // H key
            KEY_I,          // I key
            KEY_J,          // J key
            KEY_K,          // K key
            KEY_L,          // L key
            KEY_M,          // M key
            KEY_N,          // N key
            KEY_O,          // O key
            KEY_P,          // P key
            KEY_Q,          // Q key
            KEY_R,          // R key
            KEY_S,          // S key
            KEY_T,          // T key
            KEY_U,          // U key
            KEY_V,          // V key
            KEY_W,          // W key
            KEY_X,          // X key
            KEY_Y,          // Y key
            KEY_Z,          // Z key
            LSUPER,         // Left Windows key (Microsoft Natural keyboard) on Windows, left Command key on macOS
            RSUPER,         // Right Windows key (Natural keyboard) on Windows, right Command key on macOS
            APPS,           // Applications key (Natural keyboard)
            SLEEP,          // Computer Sleep key
            NUMPAD0,        // Numeric keypad 0 key
            NUMPAD1,        // Numeric keypad 1 key
            NUMPAD2,        // Numeric keypad 2 key
            NUMPAD3,        // Numeric keypad 3 key
            NUMPAD4,        // Numeric keypad 4 key
            NUMPAD5,        // Numeric keypad 5 key
            NUMPAD6,        // Numeric keypad 6 key
            NUMPAD7,        // Numeric keypad 7 key
            NUMPAD8,        // Numeric keypad 8 key
            NUMPAD9,        // Numeric keypad 9 key
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
            LSHIFT,         // Left SHIFT key
            RSHIFT,         // Right SHIFT key
            LCONTROL,       // Left CONTROL key
            RCONTROL,       // Right CONTROL key
            LALT,           // Left alt key
            RALT,           // Right alt key
            SEMICOLON,      // for US    ";:"
            PLUS,           // Plus Key   "+"
            COMMA,          // Comma Key  ","
            MINUS,          // Minus Key  "-"
            PERIOD,         // Period Key "."
            SLASH,          // for US    "/?"
            GRAVE,          // for US    "`~"
            BRACKET_LEFT,   // for US    "[{"
            BACKSLASH,      // for US    "\|"
            BRACKET_RIGHT,  // for US    "]}"
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
            A,
            B,
            X,
            Y,
            LEFT_SHOULDER,  // L1 for Apple
            LEFT_TRIGGER,   // L2 for Apple
            RIGHT_SHOULDER, // R1 for Apple
            RIGHT_TRIGGER,  // R2 for Apple
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

        class Input: public Noncopyable
        {
            friend Engine;
        public:
            virtual ~Input();

            virtual void setCursorVisible(bool visible);
            virtual bool isCursorVisible() const;

            virtual void startGamepadDiscovery();
            virtual void stopGamepadDiscovery();

            const Vector2& getCursorPosition() const { return cursorPosition; }
            bool isKeyboardKeyDown(KeyboardKey key) const { return keyboardKeyStates[static_cast<uint32_t>(key)]; }
            bool isMouseButtonDown(MouseButton button) const { return mouseButtonStates[static_cast<uint32_t>(button)]; }

            virtual void keyDown(KeyboardKey key, uint32_t modifiers);
            virtual void keyUp(KeyboardKey key, uint32_t modifiers);

            virtual void mouseDown(MouseButton button, const Vector2& position, uint32_t modifiers);
            virtual void mouseUp(MouseButton button, const Vector2& position, uint32_t modifiers);
            virtual void mouseMove(const Vector2& position, uint32_t modifiers);
            virtual void mouseRelativeMove(const Vector2& relativePosition, uint32_t modifiers);
            virtual void mouseScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers);

            virtual void touchBegin(uint64_t touchId, const Vector2& position);
            virtual void touchEnd(uint64_t touchId, const Vector2& position);
            virtual void touchMove(uint64_t touchId, const Vector2& position);
            virtual void touchCancel(uint64_t touchId, const Vector2& position);

            virtual bool showVirtualKeyboard();
            virtual bool hideVirtualKeyboard();

        protected:
            Input();

            Vector2 cursorPosition;
            bool keyboardKeyStates[static_cast<uint32_t>(KeyboardKey::KEY_COUNT)];
            bool mouseButtonStates[static_cast<uint32_t>(MouseButton::BUTTON_COUNT)];

            std::map<uint64_t, Vector2> touchPositions;
        };
    } // namespace input
} // namespace ouzel
