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
            BACK_RIGHT = 5,
            SIDE_LEFT = 6,
            SIDE_RIGHT = 7
        };

        enum class ChannelConfiguration
        {
            MONO,
            STEREO,
            QUAD,
            SURROUND51,
            SURROUND51_REAR,
            SURROUND61,
            SURROUND71
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CHANNEL_HPP
