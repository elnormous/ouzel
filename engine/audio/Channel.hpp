// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_CHANNEL_HPP
#define OUZEL_AUDIO_CHANNEL_HPP

namespace ouzel::audio
{
    enum class Channel
    {
        left = 0,
        right = 1,
        center = 2,
        lfe = 3,
        surroundLeft = 4,
        surroundRight = 5
    };

    enum class ChannelConfiguration
    {
        mono, // mono (M)
        stereo, // left (L), right (R)
        quad, // left (L), right (R), surround left (SL), surround right (SR)
        surround51 // left (L), right (R), center (C), low-frequency effects (LFE), surround left (SL), surround right (SR)
    };
}

#endif // OUZEL_AUDIO_CHANNEL_HPP
