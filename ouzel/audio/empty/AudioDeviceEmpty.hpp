// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    class Engine;

    namespace audio
    {
        class AudioDeviceEmpty: public AudioDevice
        {
            friend Audio;
        protected:
            AudioDeviceEmpty();
        };
    } // namespace audio
} // namespace ouzel
