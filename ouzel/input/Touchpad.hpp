// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_TOUCHPAD_HPP
#define OUZEL_INPUT_TOUCHPAD_HPP

#include <cstdint>
#include <unordered_map>
#include "input/Controller.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManager;

        class Touchpad final: public Controller
        {
            friend InputManager;
        public:
            Touchpad(InputManager& initInputManager, uint32_t initDeviceId, bool initScreen);

            bool isScreen() const { return screen; }

        protected:
            bool handleTouchBegin(uint64_t touchId, const Vector2F& position, float force = 1.0F);
            bool handleTouchEnd(uint64_t touchId, const Vector2F& position, float force = 1.0F);
            bool handleTouchMove(uint64_t touchId, const Vector2F& position, float force = 1.0F);
            bool handleTouchCancel(uint64_t touchId, const Vector2F& position, float force = 1.0F);

        private:
            std::unordered_map<uint64_t, Vector2F> touchPositions;
            bool screen = false;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_TOUCHPAD_HPP
