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
            class Mix: public Source
            {
            public:
                Mix() noexcept = default;
                virtual ~Mix() = default;

                Mix(const Mix&) = delete;
                Mix& operator=(const Mix&) = delete;

                Mix(Mix&&) = delete;
                Mix& operator=(Mix&&) = delete;

                virtual void getSamples(uint32_t frames, uint32_t channels, uint32_t sampleRate, std::vector<float>& samples) = 0;

            private:
                Object* parent = nullptr;
                std::vector<Object*> children;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_MIX_HPP
