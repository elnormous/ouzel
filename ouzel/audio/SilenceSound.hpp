// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_TONEDATA_HPP
#define OUZEL_AUDIO_TONEDATA_HPP

#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class SilenceSound final: public Sound
        {
        public:
            SilenceSound(Audio& initAudio, float initLength = 0.0F);

        private:
            float length;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_TONEDATA_HPP
