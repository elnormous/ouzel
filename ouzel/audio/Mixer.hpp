// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef AUDIO_MIXER_HPP
#define AUDIO_MIXER_HPP

namespace ouzel
{
    namespace audio
    {
        class Mixer
        {
        public:
            Mixer() {}

            Mixer(const Mixer&) = delete;
            Mixer& operator=(const Mixer&) = delete;

            Mixer(Mixer&&) = delete;
            Mixer& operator=(Mixer&&) = delete;
        };
    } // namespace audio
} // namespace ouzel

#endif // AUDIO_MIXER_HPP
