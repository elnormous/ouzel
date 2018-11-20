// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CHANNEL_HPP
#define OUZEL_AUDIO_CHANNEL_HPP

namespace ouzel
{
    namespace audio
    {
        enum class Channel
        {
            FRONT_LEFT = 0,
            FRONT_RIGHT = 1,
            CENTER = 2,
            LFE = 3,
            BACK_LEFT = 4,
            BACK_RIGHT = 5
        };

        enum class ChannelConfiguration
        {
            MONO, // mono
            STEREO, // left, right
            QUAD, // front left, front right, back left, back right
            SURROUND51 // front left, front right, center, LFE, back left, back right
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CHANNEL_HPP
