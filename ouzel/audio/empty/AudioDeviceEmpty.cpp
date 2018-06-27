// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "AudioDeviceEmpty.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDeviceEmpty::AudioDeviceEmpty():
            AudioDevice(Audio::Driver::EMPTY)
        {
        }
    } // namespace audio
} // namespace ouzel
