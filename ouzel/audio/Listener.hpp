// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_LISTENER_HPP
#define OUZEL_AUDIO_LISTENER_HPP

#include <cfloat>
#include "audio/AudioDevice.hpp"
#include "scene/Component.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector.hpp"

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
            Listener(Audio& initAudio);
            ~Listener();

            Listener(const Listener&) = delete;
            Listener& operator=(const Listener&) = delete;
            Listener(Listener&&) = delete;
            Listener& operator=(Listener&&) = delete;

            void setMix(Mix* newMix);

            inline const Vector3F& getPosition() const { return position; }
            inline void setPosition(const Vector3F& newPosition) { position = newPosition; }

            inline const Vector3F& getVelocity() const { return velocity; }
            inline void setVelocity(const Vector3F& newVelocity) { velocity = newVelocity; }

            inline const QuaternionF& getRotation() const { return rotation; }
            inline void setRotation(const QuaternionF& newRotation) { rotation = newRotation; }

        private:
            void updateTransform() override;

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
