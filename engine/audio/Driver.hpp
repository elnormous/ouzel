// Ouzel by Elviss Strazdins

#ifndef AUDIO_DRIVER_HPP
#define AUDIO_DRIVER_HPP

namespace ouzel::audio
{
    enum class Driver
    {
        empty,
        openAL,
        xAudio2,
        openSL,
        coreAudio,
        alsa,
        wasapi
    };
}

#endif // AUDIO_DRIVER_HPP
