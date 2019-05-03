// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_OBJECT_HPP
#define OUZEL_AUDIO_MIXER_OBJECT_HPP

#include <cstdint>
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
                Object() = default;
                virtual ~Object() {}

                Object(const Object&) = delete;
                Object& operator=(const Object&) = delete;

                Object(Object&&) = delete;
                Object& operator=(Object&&) = delete;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_OBJECT_HPP
