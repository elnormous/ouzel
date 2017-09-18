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

            float getPitch() const { return pitch; }
            void setPitch(float newPitch) { pitch = newPitch; }

            float getGain() const { return gain; }
            void setGain(float newGain) { gain = newGain; }

            float getRolloffScale() const { return rolloffScale; }
            void setRolloffScale(float newRolloffScale) { rolloffScale = newRolloffScale; }

            virtual void addRenderCommands(std::vector<AudioDevice::RenderCommand>& renderCommands) override;

        protected:
            static void setAttributes(Vector3& listenerPosition,
                                      Quaternion& listenerRotation,
                                      float& pitch,
                                      float& gain,
                                      float& rolloffFactor,
                                      float pitchScale,
                                      float gainScale,
                                      float rolloffScale);

            float pitch = 1.0f;
            float gain = 1.0f;
            float rolloffScale = 1.0f;
        };
    } // namespace audio
} // namespace ouzel
