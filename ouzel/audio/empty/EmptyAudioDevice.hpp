// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICEEMPTY_HPP
#define OUZEL_AUDIO_AUDIODEVICEEMPTY_HPP

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class EmptyAudioDevice final: public AudioDevice
        {
        public:
            explicit EmptyAudioDevice(mixer::Mixer& initMixer);
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIODEVICEEMPTY_HPP
