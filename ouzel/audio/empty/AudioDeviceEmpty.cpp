// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "AudioDeviceEmpty.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDeviceEmpty::AudioDeviceEmpty(mixer::Mixer& initMixer):
            AudioDevice(Driver::EMPTY, initMixer)
        {
        }
    } // namespace audio
} // namespace ouzel
