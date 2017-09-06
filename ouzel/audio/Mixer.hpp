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

            float getGain() const { return gain; }
            void setGain(float newGain) { gain = newGain; }

            float getRolloffFactor() const { return rolloffFactor; }
            void setRolloffFactor(float newRolloffFactor) { rolloffFactor = newRolloffFactor; }

        protected:
            float gain = 1.0f;
            float rolloffFactor = 1.0f;
        };
    } // namespace audio
} // namespace ouzel
