// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
#include "Input.h"

#ifdef __OBJC__
@class ConnectDelegate;
typedef ConnectDelegate* ConnectDelegatePtr;
#else
#include <objc/objc.h>
typedef id ConnectDelegatePtr;
#endif

namespace ouzel
{
    class Engine;
    class GamepadApple;
    
    class InputApple: public Input
    {
        friend Engine;
    public:
        virtual ~InputApple();
        
        virtual void startDiscovery() override;
        virtual void stopDiscovery() override;
        
        void handleDiscoveryCompleted();
        void handleControllerConnected(id controller);
        void handleControllerDisconnected(id controller);
        
    protected:
        InputApple();
        
        ConnectDelegatePtr _connectDelegate = nullptr;
        std::vector<std::shared_ptr<GamepadApple>> _gamepads;
        
        bool _discovering = false;
    };
}
