// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_LISTENER_HPP
#define OUZEL_AUDIO_LISTENER_HPP

#include <cfloat>
#include "audio/AudioDevice.hpp"
#include "scene/Component.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

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

        private:
            void updateTransform() override;

            Audio& audio;

            Mix* mix = nullptr;
            Vector3<float> position;
            Quaternion<float> rotation;
            bool transformDirty = true;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_LISTENER_HPP
