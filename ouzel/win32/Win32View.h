// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>

#include "../View.h"

namespace ouzel
{
    class Engine;
    class Renderer;
    
    class Win32View : public View
    {
    public:
        Win32View(const Size2& size, Renderer* renderer);
        
        void createNativeView() override;
        void destroyNativeView() override;
        bool processEvents();
        
    private:
        HWND _window;
        
    };
}