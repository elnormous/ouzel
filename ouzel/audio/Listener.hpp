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

        private:
            void updateTransform() override;

            Audio& audio;

            Mix* mix = nullptr;
            Vector3 position;
            Quaternion rotation;
            bool transformDirty = true;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_LISTENER_HPP
