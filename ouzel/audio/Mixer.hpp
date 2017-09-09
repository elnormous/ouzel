// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Noncopyable.hpp"
#include "audio/SoundInput.hpp"
#include "audio/SoundOutput.hpp"

namespace ouzel
{
    namespace audio
    {
        class Mixer: public Noncopyable, public SoundInput, public SoundOutput
        {
        public:
            Mixer();
            virtual ~Mixer();

            float getGain() const { return gain; }
            void setGain(float newGain) { gain = newGain; }

            float getRolloffFactor() const { return rolloffFactor; }
            void setRolloffFactor(float newRolloffFactor) { rolloffFactor = newRolloffFactor; }

            virtual AudioDevice::RenderCommand getRenderCommand() override;

        protected:
            static bool render(uint32_t frames,
                               uint16_t channels,
                               uint32_t sampleRate,
                               Vector3& sourcePosition,
                               float& sourceRolloffFactor,
                               float& sourceMinDistance,
                               float& sourceMaxDistance,
                               std::vector<float>& result,
                               float gain);

            float gain = 1.0f;
            float rolloffFactor = 1.0f;
        };
    } // namespace audio
} // namespace ouzel
