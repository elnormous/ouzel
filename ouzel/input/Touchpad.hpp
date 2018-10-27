// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <unordered_map>
#include "input/Controller.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManager;

        class Touchpad final: public Controller
        {
            friend InputManager;
        public:
            Touchpad(InputManager& initInputManager, uint32_t initDeviceId);

        protected:
            bool handleTouchBegin(uint64_t touchId, const Vector2& position, float force = 1.0F);
            bool handleTouchEnd(uint64_t touchId, const Vector2& position, float force = 1.0F);
            bool handleTouchMove(uint64_t touchId, const Vector2& position, float force = 1.0F);
            bool handleTouchCancel(uint64_t touchId, const Vector2& position, float force = 1.0F);

        private:
            std::unordered_map<uint64_t, Vector2> touchPositions;
        };
    } // namespace input
} // namespace ouzel
