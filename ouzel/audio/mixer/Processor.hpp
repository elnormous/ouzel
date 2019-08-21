// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_PROCESSOR_HPP
#define OUZEL_AUDIO_MIXER_PROCESSOR_HPP

#include "audio/mixer/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Processor
            {
            public:
                Processor() noexcept = default;
                virtual ~Processor() = default;

                Processor(const Processor&) = delete;
                Processor& operator=(const Processor&) = delete;

                Processor(Processor&&) = delete;
                Processor& operator=(Processor&&) = delete;

                virtual void process(uint32_t frames, uint32_t channels, uint32_t sampleRate,
                                     std::vector<float>& samples) = 0;

                inline auto isEnabled() const noexcept { return enabled; }
                inline void setEnabled(bool newEnabled) { enabled = newEnabled; }

            private:
                bool enabled = true;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_PROCESSOR_HPP
