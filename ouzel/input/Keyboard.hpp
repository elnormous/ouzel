// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_KEYBOARD_HPP
#define OUZEL_INPUT_KEYBOARD_HPP

#include <cstdint>
#include "input/Controller.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManager;

        class Keyboard final: public Controller
        {
            friend InputManager;
        public:
            enum class Key
            {
                Unknown,
                Cancel, // Control-break processing
                Backspace, // Backspace key
                Tab, // Tab key
                Clear, // Clear key
                Enter, // Enter key
                Pause, // Pause key
                CapsLock, // Caps Lock key
                Kana, // Japanese kana key
                Hanja, // Japanese Hanja key
                Kanji, // Japanese Kanji key
                Yen, // Japanese Yen key
                Ro, // Japanese Ro key
                Katakana, // Japanese Katakana key
                Hiragana, // Japanese Hiragana key
                KatakanaHiragana, // Japanese Katakana / Hiragana key
                Henkan, // Japanese Henkan key
                Muhenkan, // Japanese non-conversion key
                NumpadJpcomma, // Japanese numeric keypad comma key
                Escape, // Esc key
                Space, // Spacebar
                PageUp, // Page Up key
                PageDown, // Page Down key
                End, // End key
                Home, // Home key
                Left, // Left Arrow key
                Up, // Up Arrow key
                Right, // Right Arrow key
                Down, // Down Arrow key
                Select, // Select key
                Print, // Print key
                Execute, // Execute key
                PrintScreen, // Print Screen key
                Insert, // Ins key
                Del, // Delete key
                Equal, // Equal key
                Help, // Help key
                Num0, // 0 key
                Num1, // 1 key
                Num2, // 2 key
                Num3, // 3 key
                Num4, // 4 key
                Num5, // 5 key
                Num6, // 6 key
                Num7, // 7 key
                Num8, // 8 key
                Num9, // 9 key
                A, // A key
                B, // B key
                C, // C key
                D, // D key
                E, // E key
                F, // F key
                G, // G key
                H, // H key
                I, // I key
                J, // J key
                K, // K key
                L, // L key
                M, // M key
                N, // N key
                O, // O key
                P, // P key
                Q, // Q key
                R, // R key
                S, // S key
                T, // T key
                U, // U key
                V, // V key
                W, // W key
                X, // X key
                Y, // Y key
                Z, // Z key
                LeftSuper, // Left Windows key (Microsoft Natural keyboard) on Windows, left Command key on macOs
                RightSuper, // Right Windows key (Natural keyboard) on Windows, right Command key on macOs
                Menu, // Applications key (Natural keyboard)
                Sleep, // Computer sleep key
                Power, // Computer power key
                Numpad0, // Numeric keypad 0 key
                Numpad1, // Numeric keypad 1 key
                Numpad2, // Numeric keypad 2 key
                Numpad3, // Numeric keypad 3 key
                Numpad4, // Numeric keypad 4 key
                Numpad5, // Numeric keypad 5 key
                Numpad6, // Numeric keypad 6 key
                Numpad7, // Numeric keypad 7 key
                Numpad8, // Numeric keypad 8 key
                Numpad9, // Numeric keypad 9 key
                NumpadMultiply, // Numeric keypad multiply key
                NumpadPlus, // Numeric keypad plus key
                NumpadSeparator, // Numeric keypad comma key
                NumpadMinus, // Numeric keypad minus key
                NumpadDecimal, // Numeric keypad decimal key
                NumpadDivide, // Numeric keypad divide key
                NumpadEnter, // Numeric keypad enter key
                NumpadEqual, // Numeric keypad comma key
                NumpadPlusminus, // Numeric keypad plus/minus key
                NumpadLeftParen, // Numeric keypad comma key
                NumpadRightParen, // Numeric keypad comma key
                F1, // F1 key
                F2, // F2 key
                F3, // F3 key
                F4, // F4 key
                F5, // F5 key
                F6, // F6 key
                F7, // F7 key
                F8, // F8 key
                F9, // F9 key
                F10, // F10 key
                F11, // F11 key
                F12, // F12 key
                F13, // F13 key
                F14, // F14 key
                F15, // F15 key
                F16, // F16 key
                F17, // F17 key
                F18, // F18 key
                F19, // F19 key
                F20, // F20 key
                F21, // F21 key
                F22, // F22 key
                F23, // F23 key
                F24, // F24 key
                NumLock, // Num Lock key
                ScrollLock, // Scroll Lock key
                LeftShift, // Left Shift key
                RightShift, // Right Shift key
                LeftControl, // Left Control key
                RightControl, // Right Control key
                LeftAlt, // Left alt key
                RightAlt, // Right alt key
                ModeChange, // Mode change key
                Semicolon, // for US ";:"
                Plus, // Plus Key "+"
                Comma, // Comma Key ","
                Minus, // Minus Key "-"
                Period, // Period Key "."
                Slash, // for US "/?"
                Grave, // for US "`~"
                LeftBracket, // for US "[{"
                Backslash, // for US "\|"
                RightBracket, // for US "]}"
                Quote, // for US "'""
                OemAx, // for Japan "AX"
                IntlBackslash, // "<>" or "\|"
                Crsel, // CrSel key
                Exsel, // ExSel key
                App1, // Launch application 1 key
                App2, // Launch application 2 key
                Back, // AC back key
                Forward, // AC forward key
                Refresh, // AC refresh key
                Stop, // AC stop key
                Search, // AC search key
                Bookmarks, // AC bookmarks key
                Homepage, // AC home key
                Mute, // Audio mute button
                VolumeDown, // Audio volume down button
                VolumeUp, // Audio volume up button
                AudioPlay, // Play/pause media key
                AudioStop, // Stop media key
                AudioNext, // Play next media key
                AudioPrevious, // Play previous media key
                MediaSelect, // Launch media select key
                Mail, // Mail function key
                Count
            };

            Keyboard(InputManager& initInputManager, DeviceId initDeviceId);

            inline auto isKeyDown(Key key) const noexcept { return keyStates[static_cast<uint32_t>(key)]; }

        protected:
            bool handleKeyPress(Keyboard::Key key);
            bool handleKeyRelease(Keyboard::Key key);

        private:
            bool keyStates[static_cast<uint32_t>(Key::Count)]{false};
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_KEYBOARD_HPP
