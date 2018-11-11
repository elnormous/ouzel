// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_HPP
#define OUZEL_AUDIO_MIXER_HPP

#include "audio/SoundInput.hpp"
#include "audio/SoundOutput.hpp"

namespace ouzel
{
    namespace audio
    {
        class Mixer: public SoundInput, public SoundOutput
        {
        public:
            Mixer();
            virtual ~Mixer();

            Mixer(const Mixer&) = delete;
            Mixer& operator=(const Mixer&) = delete;

            Mixer(Mixer&&) = delete;
            Mixer& operator=(Mixer&&) = delete;

            inline float getPitch() const { return pitch; }
            inline void setPitch(float newPitch) { pitch = newPitch; }

            inline float getGain() const { return gain; }
            inline void setGain(float newGain) { gain = newGain; }

            inline float getRolloffScale() const { return rolloffScale; }
            inline void setRolloffScale(float newRolloffScale) { rolloffScale = newRolloffScale; }

            void addRenderCommands(std::vector<AudioDevice::RenderCommand>& renderCommands) override;

        protected:
            static void setAttributes(Vector3& listenerPosition,
                                      Quaternion& listenerRotation,
                                      float& pitch,
                                      float& gain,
                                      float& rolloffFactor,
                                      float pitchScale,
                                      float gainScale,
                                      float rolloffScale);

            float pitch = 1.0F;
            float gain = 1.0F;
            float rolloffScale = 1.0F;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_HPP
