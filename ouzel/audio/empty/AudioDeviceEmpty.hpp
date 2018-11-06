// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIODEVICEEMPTY_HPP
#define OUZEL_AUDIODEVICEEMPTY_HPP

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

#endif // OUZEL_AUDIODEVICEEMPTY_HPP
