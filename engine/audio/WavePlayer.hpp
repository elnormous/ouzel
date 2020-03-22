// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_WAVEPLAYER_HPP
#define OUZEL_AUDIO_WAVEPLAYER_HPP

#include "audio/Source.hpp"
#include "audio/Sound.hpp"

namespace ouzel
{
    namespace audio
    {
        class VorbisData;

        class WavePlayer final: public Source
        {
        public:
            explicit WavePlayer(const Sound*)
            {
            }

            void getSamples(std::uint32_t, std::uint32_t, std::uint32_t, std::vector<float>&) override
            {
            }

        private:
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_WAVEPLAYER_HPP
