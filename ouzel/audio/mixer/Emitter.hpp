// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_EMITTER_HPP
#define OUZEL_AUDIO_MIXER_EMITTER_HPP

#include <cstdint>

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Emitter
            {
            public:
                Emitter() noexcept = default;
                virtual ~Emitter() = default;

                Emitter(const Emitter&) = delete;
                Emitter& operator=(const Emitter&) = delete;

                Emitter(Emitter&&) = delete;
                Emitter& operator=(Emitter&&) = delete;

                virtual void play() = 0;
                virtual void stop(bool shouldReset) = 0;
                
                virtual void getSamples(uint32_t frames, uint32_t channels, uint32_t sampleRate, std::vector<float>& samples) = 0;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_EMITTER_HPP
