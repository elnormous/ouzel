// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

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
