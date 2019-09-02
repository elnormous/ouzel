// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SILENCESOUND_HPP
#define OUZEL_AUDIO_SILENCESOUND_HPP

#include "audio/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        class Silence final: public Source
        {
        public:
            Silence(float initLength = 0.0F);

            void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples) override;

        private:
            float length;
            uint32_t position = 0;
            bool playing = false;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SILENCESOUND_HPP
