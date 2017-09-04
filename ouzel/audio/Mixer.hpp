// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundInput.hpp"
#include "audio/SoundOutput.hpp"

namespace ouzel
{
    namespace audio
    {
        class MixerResource;

        class Mixer: public SoundInput, public SoundOutput
        {
        public:
            Mixer();
            virtual ~Mixer();

        private:
            MixerResource* resource = nullptr;
        };
    } // namespace audio
} // namespace ouzel
