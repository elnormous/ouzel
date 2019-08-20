// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_OBJECT_HPP
#define OUZEL_AUDIO_MIXER_OBJECT_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include "math/Quaternion.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Object
            {
            public:
                Object() noexcept = default;
                virtual ~Object() = default;

                virtual void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples) = 0;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_OBJECT_HPP
