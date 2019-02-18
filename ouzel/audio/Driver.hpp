// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef AUDIO_DRIVER_HPP
#define AUDIO_DRIVER_HPP

namespace ouzel
{
    namespace audio
    {
        enum class Driver
        {
            EMPTY,
            OPENAL,
            DIRECTSOUND,
            XAUDIO2,
            OPENSL,
            COREAUDIO,
            ALSA,
            WASAPI
        };
    } // namespace audio
} // namespace ouzel

#endif // AUDIO_DRIVER_HPP
