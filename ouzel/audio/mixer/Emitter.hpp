// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_MIX_HPP
#define OUZEL_AUDIO_MIXER_MIX_HPP

#include <cstdint>
#include <vector>
#include "audio/mixer/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Emitter: public Source
            {
            public:
                Emitter() noexcept = default;
                virtual ~Emitter() = default;

                Emitter(const Emitter&) = delete;
                Emitter& operator=(const Emitter&) = delete;

                Emitter(Emitter&&) = delete;
                Emitter& operator=(Emitter&&) = delete;

                virtual void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples) = 0;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_MIX_HPP
