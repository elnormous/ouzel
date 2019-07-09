// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CHANNEL_HPP
#define OUZEL_AUDIO_CHANNEL_HPP

namespace ouzel
{
    namespace audio
    {
        enum class Channel
        {
            Left = 0,
            Right = 1,
            Center = 2,
            LFE = 3,
            SurroundLeft = 4,
            SurroundRight = 5
        };

        enum class ChannelConfiguration
        {
            Mono, // mono (M)
            Stereo, // left (L), right (R)
            Quad, // left (L), right (R), surround left (SL), surround right (SR)
            Surround51 // left (L), right (R), center (C), low-frequency effects (LFE), surround left (SL), surround right (SR)
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CHANNEL_HPP
