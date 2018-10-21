// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include "input/Controller.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManager;

        class Keyboard: public Controller
        {
            friend InputManager;
        public:
            enum class Key
            {
                NONE,
                CANCEL,             // Control-break processing
                BACKSPACE,          // BACKSPACE key
                TAB,                // TAB key
                CLEAR,              // CLEAR key
                ENTER,              // ENTER key
                PAUSE,              // PAUSE key
                CAPS_LOCK,          // CAPS LOCK key
                KANA,               // Japanese kana key
                HANJA,              // Japanese Hanja key
                KANJI,              // Japanese Kanji key
                YEN,                // Japanese Yen key
                RO,                 // Japanese Ro key
                KATAKANA,           // Japanese Katakana key
                HIRAGANA,           // Japanese Hiragana key
                KATAKANA_HIRAGANA,  // Japanese Katakana / Hiragana key
                HENKAN,             // Japanese Henkan key
                MUHENKAN,           // Japanese non-conversion key
                NUMPAD_JPCOMMA,     // Japanese numeric keypad comma key
                ESCAPE,             // ESC key
                SPACE,              // SPACEBAR
                PAGE_UP,            // PAGE UP key
                PAGE_DOWN,          // PAGE DOWN key
                END,                // END key
                HOME,               // HOME key
                LEFT,               // LEFT ARROW key
                UP,                 // UP ARROW key
                RIGHT,              // RIGHT ARROW key
                DOWN,               // DOWN ARROW key
                SELECT,             // SELECT key
                PRINT,              // PRINT key
                EXECUTE,            // EXECUTE key
                PRINT_SCREEN,       // PRINT SCREEN key
                INSERT,             // INS key
                DEL,                // DELETE key
                EQUAL,              // EQUAL key
                HELP,               // HELP key
                NUM_0,              // 0 key
                NUM_1,              // 1 key
                NUM_2,              // 2 key
                NUM_3,              // 3 key
                NUM_4,              // 4 key
                NUM_5,              // 5 key
                NUM_6,              // 6 key
                NUM_7,              // 7 key
                NUM_8,              // 8 key
                NUM_9,              // 9 key
                A,                  // A key
                B,                  // B key
                C,                  // C key
                D,                  // D key
                E,                  // E key
                F,                  // F key
                G,                  // G key
                H,                  // H key
                I,                  // I key
                J,                  // J key
                K,                  // K key
                L,                  // L key
                M,                  // M key
                N,                  // N key
                O,                  // O key
                P,                  // P key
                Q,                  // Q key
                R,                  // R key
                S,                  // S key
                T,                  // T key
                U,                  // U key
                V,                  // V key
                W,                  // W key
                X,                  // X key
                Y,                  // Y key
                Z,                  // Z key
                LEFT_SUPER,         // Left Windows key (Microsoft Natural keyboard) on Windows, left Command key on macOS
                RIGHT_SUPER,        // Right Windows key (Natural keyboard) on Windows, right Command key on macOS
                MENU,               // Applications key (Natural keyboard)
                SLEEP,              // Computer sleep key
                POWER,              // Computer power key
                NUMPAD_0,           // Numeric keypad 0 key
                NUMPAD_1,           // Numeric keypad 1 key
                NUMPAD_2,           // Numeric keypad 2 key
                NUMPAD_3,           // Numeric keypad 3 key
                NUMPAD_4,           // Numeric keypad 4 key
                NUMPAD_5,           // Numeric keypad 5 key
                NUMPAD_6,           // Numeric keypad 6 key
                NUMPAD_7,           // Numeric keypad 7 key
                NUMPAD_8,           // Numeric keypad 8 key
                NUMPAD_9,           // Numeric keypad 9 key
                NUMPAD_MULTIPLY,    // Numeric keypad multiply key
                NUMPAD_PLUS,        // Numeric keypad plus key
                NUMPAD_SEPARATOR,   // Numeric keypad comma key
                NUMPAD_MINUS,       // Numeric keypad minus key
                NUMPAD_DECIMAL,     // Numeric keypad decimal key
                NUMPAD_DIVIDE,      // Numeric keypad divide key
                NUMPAD_ENTER,       // Numeric keypad enter key
                NUMPAD_EQUAL,       // Numeric keypad comma key
                NUMPAD_PLUSMINUS,   // Numeric keypad plus/minus key
                NUMPAD_LEFT_PAREN,  // Numeric keypad comma key
                NUMPAD_RIGHT_PAREN, // Numeric keypad comma key
                F1,                 // F1 key
                F2,                 // F2 key
                F3,                 // F3 key
                F4,                 // F4 key
                F5,                 // F5 key
                F6,                 // F6 key
                F7,                 // F7 key
                F8,                 // F8 key
                F9,                 // F9 key
                F10,                // F10 key
                F11,                // F11 key
                F12,                // F12 key
                F13,                // F13 key
                F14,                // F14 key
                F15,                // F15 key
                F16,                // F16 key
                F17,                // F17 key
                F18,                // F18 key
                F19,                // F19 key
                F20,                // F20 key
                F21,                // F21 key
                F22,                // F22 key
                F23,                // F23 key
                F24,                // F24 key
                NUM_LOCK,           // NUM LOCK key
                SCROLL_LOCK,        // SCROLL LOCK key
                LEFT_SHIFT,         // Left SHIFT key
                RIGHT_SHIFT,        // Right SHIFT key
                LEFT_CONTROL,       // Left CONTROL key
                RIGHT_CONTROL,      // Right CONTROL key
                LEFT_ALT,           // Left alt key
                RIGHT_ALT,          // Right alt key
                MODE_CHANGE,        // Mode change key
                SEMICOLON,          // for US    ";:"
                PLUS,               // Plus Key   "+"
                COMMA,              // Comma Key  ","
                MINUS,              // Minus Key  "-"
                PERIOD,             // Period Key "."
                SLASH,              // for US    "/?"
                GRAVE,              // for US    "`~"
                LEFT_BRACKET,       // for US    "[{"
                BACKSLASH,          // for US    "\|"
                RIGHT_BRACKET,      // for US    "]}"
                QUOTE,              // for US    "'""
                OEM_AX,             // for Japan "AX"
                INTL_BACKSLASH,     // "<>" or "\|"
                CRSEL,              // CrSel key
                EXSEL,              // ExSel key
                BACK,               // AC back key
                FORWARD,            // AC forward key
                REFRESH,            // AC refresh key
                STOP,               // AC stop key
                SEARCH,             // AC search key
                BOOKMARKS,          // AC bookmarks key
                HOMEPAGE,           // AC home key
                MUTE,               // Audio mute button
                VOLUME_DOWN,        // Audio volume down button
                VOLUME_UP,          // Audio volume up button
                AUDIO_PLAY,         // Play/pause media key
                AUDIO_STOP,         // Stop media key
                AUDIO_NEXT,         // Play next media key
                AUDIO_PREVIOUS,     // Play previous media key
                MEDIA_SELECT,       // Launch media select key
                MAIL,               // Mail function key
                COUNT
            };

            Keyboard(InputManager& initInputManager, uint32_t initDeviceId);

            inline bool isKeyDown(Key key) const { return keyStates[static_cast<uint32_t>(key)]; }

        protected:
            bool handleKeyPress(Keyboard::Key key);
            bool handleKeyRelease(Keyboard::Key key);

        private:
            bool keyStates[static_cast<uint32_t>(Key::COUNT)];
        };
    } // namespace input
} // namespace ouzel
