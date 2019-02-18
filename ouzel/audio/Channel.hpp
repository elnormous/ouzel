// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CHANNEL_HPP
#define OUZEL_AUDIO_CHANNEL_HPP

namespace ouzel
{
    namespace audio
    {
        enum class Channel
        {
            LEFT = 0,
            RIGHT = 1,
            CENTER = 2,
            LFE = 3,
            SURROUND_LEFT = 4,
            SURROUND_RIGHT = 5
        };

        enum class ChannelConfiguration
        {
            MONO, // mono (M)
            STEREO, // left (L), right (R)
            QUAD, // left (L), right (R), surround left (SL), surround right (SR)
            SURROUND51 // left (L), right (R), center (C), low-frequency effects (LFE), surround left (SL), surround right (SR)
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CHANNEL_HPP
