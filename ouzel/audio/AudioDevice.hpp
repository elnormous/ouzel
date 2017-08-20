// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDevice
        {
            friend Audio;
        public:
            virtual ~AudioDevice();

        protected:
            AudioDevice(Audio::Driver aDriver);

            Audio::Driver driver;
        };
    } // namespace audio
} // namespace ouzel
