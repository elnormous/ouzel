// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef AUDIO_DRIVER_HPP
#define AUDIO_DRIVER_HPP

namespace ouzel
{
    namespace audio
    {
        enum class Driver
        {
            empty,
            openAL,
            directSound,
            xAudio2,
            openSL,
            coreAudio,
            alsa,
            wasapi
        };
    } // namespace audio
} // namespace ouzel

#endif // AUDIO_DRIVER_HPP
