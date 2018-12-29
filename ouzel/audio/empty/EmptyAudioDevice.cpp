// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "EmptyAudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        EmptyAudioDevice::EmptyAudioDevice(mixer::Mixer& initMixer):
            AudioDevice(Driver::EMPTY, initMixer)
        {
        }
    } // namespace audio
} // namespace ouzel
