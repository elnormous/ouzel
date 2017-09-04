// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundOutput.hpp"

namespace ouzel
{
    namespace audio
    {
        class MixerResource;

        class Mixer: public SoundOutput
        {
        public:
            Mixer();
            virtual ~Mixer();

        private:
            MixerResource* resource = nullptr;
        };
    } // namespace audio
} // namespace ouzel
