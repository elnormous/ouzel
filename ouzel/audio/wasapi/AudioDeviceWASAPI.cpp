// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "AudioDeviceWASAPI.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDeviceWASAPI::AudioDeviceWASAPI(Mixer& initMixer):
            AudioDevice(Driver::WASAPI, initMixer)
        {
        }
    } // namespace audio
} // namespace ouzel
