// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceEmpty final: public AudioDevice
        {
            friend Audio;
        protected:
            AudioDeviceEmpty();
        };
    } // namespace audio
} // namespace ouzel
