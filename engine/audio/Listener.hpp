// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_LISTENER_HPP
#define OUZEL_AUDIO_LISTENER_HPP

#include <cfloat>
#include "AudioDevice.hpp"
#include "../scene/Component.hpp"
#include "../math/Quaternion.hpp"
#include "../math/Vector.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Mix;

        class Listener final: public scene::Component
        {
            friend Mix;
        public:
            explicit Listener(Audio& initAudio);
            ~Listener() override;

            Listener(const Listener&) = delete;
            Listener& operator=(const Listener&) = delete;
            Listener(Listener&&) = delete;
            Listener& operator=(Listener&&) = delete;

            void setMix(Mix* newMix);

            inline auto& getPosition() const noexcept { return position; }
            inline void setPosition(const Vector3F& newPosition) { position = newPosition; }

            inline auto& getVelocity() const noexcept { return velocity; }
            inline void setVelocity(const Vector3F& newVelocity) { velocity = newVelocity; }

            inline auto& getRotation() const noexcept { return rotation; }
            inline void setRotation(const QuaternionF& newRotation) { rotation = newRotation; }

        private:
            void updateTransform() final;

            Audio& audio;

            Mix* mix = nullptr;
            Vector3F position;
            Vector3F velocity;
            QuaternionF rotation;
            bool transformDirty = true;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_LISTENER_HPP
