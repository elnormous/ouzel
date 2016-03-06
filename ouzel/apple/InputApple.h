// Copyright (C) 2016 Elviss Strazdins
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
    namespace input
    {
        class GamepadApple;
        
        class InputApple: public Input
        {
            friend Engine;
        public:
            virtual ~InputApple();
            
            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;
            
            virtual void startGamepadDiscovery() override;
            virtual void stopGamepadDiscovery() override;
            
            void handleGamepadDiscoveryCompleted();
            void handleGamepadConnected(id controller);
            void handleGamepadDisconnected(id controller);
            
        protected:
            InputApple();
            
            ConnectDelegatePtr _connectDelegate = nullptr;
            std::vector<std::shared_ptr<GamepadApple>> _gamepads;
            
            bool _discovering = false;
        };
    } // namespace input
} // namespace ouzel
