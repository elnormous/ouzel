// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector2.h"

namespace ouzel
{
    enum class KeyCode
    {
        NONE             = 0x00,
        LBUTTON          = 0x01,  // Left mouse button
        RBUTTON          = 0x02,  // Right mouse button
        CANCEL           = 0x03,  // Control-break processing
        MBUTTON          = 0x04,  // Middle mouse button (three-button mouse)
        XBUTTON1         = 0x05,  // Windows 2000/XP: X1 mouse button
        XBUTTON2         = 0x06,  // Windows 2000/XP: X2 mouse button
        BACK             = 0x08,  // BACKSPACE key
        TAB              = 0x09,  // TAB key
        CLEAR            = 0x0C,  // CLEAR key
        RETURN           = 0x0D,  // ENTER key
        SHIFT            = 0x10,  // SHIFT key
        CONTROL          = 0x11,  // CTRL key
        MENU             = 0x12,  // ALT key
        PAUSE            = 0x13,  // PAUSE key
        CAPITAL          = 0x14,  // CAPS LOCK key
        KANA             = 0x15,  // IME Kana mode
        HANGUEL          = 0x15,  // IME Hanguel mode (maintained for compatibility use HANGUL)
        HANGUL           = 0x15,  // IME Hangul mode
        JUNJA            = 0x17,  // IME Junja mode
        FINAL            = 0x18,  // IME final mode
        HANJA            = 0x19,  // IME Hanja mode
        KANJI            = 0x19,  // IME Kanji mode
        ESCAPE           = 0x1B,  // ESC key
        CONVERT          = 0x1C,  // IME convert
        NONCONVERT       = 0x1D,  // IME nonconvert
        ACCEPT           = 0x1E,  // IME accept
        MODECHANGE       = 0x1F,  // IME mode change request
        SPACE            = 0x20,  // SPACEBAR
        PRIOR            = 0x21,  // PAGE UP key
        NEXT             = 0x22,  // PAGE DOWN key
        END              = 0x23,  // END key
        HOME             = 0x24,  // HOME key
        LEFT             = 0x25,  // LEFT ARROW key
        UP               = 0x26,  // UP ARROW key
        RIGHT            = 0x27,  // RIGHT ARROW key
        DOWN             = 0x28,  // DOWN ARROW key
        SELECT           = 0x29,  // SELECT key
        PRINT            = 0x2A,  // PRINT key
        EXECUT           = 0x2B,  // EXECUTE key
        SNAPSHOT         = 0x2C,  // PRINT SCREEN key
        INSERT           = 0x2D,  // INS key
        DELETE           = 0x2E,  // DEL key
        HELP             = 0x2F,  // HELP key
        KEY_0            = 0x30,  // 0 key
        KEY_1            = 0x31,  // 1 key
        KEY_2            = 0x32,  // 2 key
        KEY_3            = 0x33,  // 3 key
        KEY_4            = 0x34,  // 4 key
        KEY_5            = 0x35,  // 5 key
        KEY_6            = 0x36,  // 6 key
        KEY_7            = 0x37,  // 7 key
        KEY_8            = 0x38,  // 8 key
        KEY_9            = 0x39,  // 9 key
        KEY_A            = 0x41,  // A key
        KEY_B            = 0x42,  // B key
        KEY_C            = 0x43,  // C key
        KEY_D            = 0x44,  // D key
        KEY_E            = 0x45,  // E key
        KEY_F            = 0x46,  // F key
        KEY_G            = 0x47,  // G key
        KEY_H            = 0x48,  // H key
        KEY_I            = 0x49,  // I key
        KEY_J            = 0x4A,  // J key
        KEY_K            = 0x4B,  // K key
        KEY_L            = 0x4C,  // L key
        KEY_M            = 0x4D,  // M key
        KEY_N            = 0x4E,  // N key
        KEY_O            = 0x4F,  // O key
        KEY_P            = 0x50,  // P key
        KEY_Q            = 0x51,  // Q key
        KEY_R            = 0x52,  // R key
        KEY_S            = 0x53,  // S key
        KEY_T            = 0x54,  // T key
        KEY_U            = 0x55,  // U key
        KEY_V            = 0x56,  // V key
        KEY_W            = 0x57,  // W key
        KEY_X            = 0x58,  // X key
        KEY_Y            = 0x59,  // Y key
        KEY_Z            = 0x5A,  // Z key
        LWIN             = 0x5B,  // Left Windows key (MicrosoftŽ NaturalŽ keyboard)
        RWIN             = 0x5C,  // Right Windows key (Natural keyboard)
        APPS             = 0x5D,  // Applications key (Natural keyboard)
        SLEEP            = 0x5F,  // Computer Sleep key
        NUMPAD0          = 0x60,  // Numeric keypad 0 key
        NUMPAD1          = 0x61,  // Numeric keypad 1 key
        NUMPAD2          = 0x62,  // Numeric keypad 2 key
        NUMPAD3          = 0x63,  // Numeric keypad 3 key
        NUMPAD4          = 0x64,  // Numeric keypad 4 key
        NUMPAD5          = 0x65,  // Numeric keypad 5 key
        NUMPAD6          = 0x66,  // Numeric keypad 6 key
        NUMPAD7          = 0x67,  // Numeric keypad 7 key
        NUMPAD8          = 0x68,  // Numeric keypad 8 key
        NUMPAD9          = 0x69,  // Numeric keypad 9 key
        MULTIPLY         = 0x6A,  // Multiply key
        ADD              = 0x6B,  // Add key
        SEPARATOR        = 0x6C,  // Separator key
        SUBTRACT         = 0x6D,  // Subtract key
        DECIMAL          = 0x6E,  // Decimal key
        DIVIDE           = 0x6F,  // Divide key
        F1               = 0x70,  // F1 key
        F2               = 0x71,  // F2 key
        F3               = 0x72,  // F3 key
        F4               = 0x73,  // F4 key
        F5               = 0x74,  // F5 key
        F6               = 0x75,  // F6 key
        F7               = 0x76,  // F7 key
        F8               = 0x77,  // F8 key
        F9               = 0x78,  // F9 key
        F10              = 0x79,  // F10 key
        F11              = 0x7A,  // F11 key
        F12              = 0x7B,  // F12 key
        F13              = 0x7C,  // F13 key
        F14              = 0x7D,  // F14 key
        F15              = 0x7E,  // F15 key
        F16              = 0x7F,  // F16 key
        F17              = 0x80,  // F17 key
        F18              = 0x81,  // F18 key
        F19              = 0x82,  // F19 key
        F20              = 0x83,  // F20 key
        F21              = 0x84,  // F21 key
        F22              = 0x85,  // F22 key
        F23              = 0x86,  // F23 key
        F24              = 0x87,  // F24 key
        NUMLOCK          = 0x90,  // NUM LOCK key
        SCROLL           = 0x91,  // SCROLL LOCK key
        LSHIFT           = 0xA0,  // Left SHIFT key
        RSHIFT           = 0xA1,  // Right SHIFT key
        LCONTROL         = 0xA2,  // Left CONTROL key
        RCONTROL         = 0xA3,  // Right CONTROL key
        LMENU            = 0xA4,  // Left MENU key
        RMENU            = 0xA5,  // Right MENU key
        OEM_1            = 0xBA,  // for US    ";:"
        PLUS             = 0xBB,  // Plus Key   "+"
        COMMA            = 0xBC,  // Comma Key  ","
        MINUS            = 0xBD,  // Minus Key  "-"
        PERIOD           = 0xBE,  // Period Key "."
        OEM_2            = 0xBF,  // for US    "/?"
        OEM_3            = 0xC0,  // for US    "`~"
        OEM_4            = 0xDB,  // for US    "[{"
        OEM_5            = 0xDC,  // for US    "\|"
        OEM_6            = 0xDD,  // for US    "]}"
        OEM_7            = 0xDE,  // for US    "'""
        OEM_8            = 0xDF,  // None
        OEM_AX           = 0xE1,  // for Japan "AX"
        OEM_102          = 0xE2,  // "<>" or "\|"
        ATTN             = 0xF6,  // Attn key
        CRSEL            = 0xF7,  // CrSel key
        EXSEL            = 0xF8,  // ExSel key
        EREOF            = 0xF9,  // Erase EOF key
        PLAY             = 0xFA,  // Play key
        ZOOM             = 0xFB,  // Zoom key
        PA1              = 0xFD,  // PA1 key
        OEM_CLEAR        = 0xFE,   // Clear key
        
        KEY_CODES_COUNT  = 0xFF // this is not a key, but the amount of keycodes there are.
    };
    
    struct Event
    {
        enum class Type
        {
            KEY_DOWN,
            KEY_UP,
            MOUSE_DOWN,
            MOUSE_UP,
            MOUSE_SCROLL,
            MOUSE_MOVE,
            MOUSE_DRAG
        };
        
        Type type;
        
        uint32_t key;
        uint32_t mouseButton;
        Vector2 mousePosition;
        Vector2 mouseScroll;
        
        bool shiftDown;
        bool controlDown;
        bool commandDown;
        bool functionDown;
    };
    
    class EventHandler
    {
    public:
        virtual bool handleEvent(const Event& event) = 0;
        virtual void update(float delta) = 0;
    };
}
