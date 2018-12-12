// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cmath>
#include <cstring>
#include "Pitch.hpp"
#include "Audio.hpp"
#include "math/MathUtils.hpp"
#include "smbPitchShift.hpp"

namespace ouzel
{
    namespace audio
    {
        static constexpr float MIN_PITCH = 0.5F;
        static constexpr float MAX_PITCH = 2.0F;

        class PitchProcessor final: public Processor
        {
        public:
            PitchProcessor(float initPitch):
                pitch(clamp(initPitch, MIN_PITCH, MAX_PITCH))
            {
            }

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override
            {
                channelSamples.resize(frames);
                pitchShift.resize(channels);

                for (uint16_t channel = 0; channel < channels; ++channel)
                {
                    for (uint32_t sample = 0; sample < frames; ++sample)
                        channelSamples[sample] = samples[sample * channels + channel];

                    pitchShift[channel].process(pitch, frames, 1024, 4, static_cast<float>(sampleRate), channelSamples.data(), channelSamples.data());

                    for (uint32_t sample = 0; sample < frames; ++sample)
                        samples[sample * channels + channel] = channelSamples[sample];
                }
            }

            void setPitch(float newPitch)
            {
                pitch = clamp(newPitch, MIN_PITCH, MAX_PITCH);
            }

        private:
            float pitch = 1.0f;
            std::vector<float> channelSamples;
            std::vector<smb::PitchShift> pitchShift;
        };

        Pitch::Pitch(Audio& initAudio, float initPitch):
            Filter(initAudio,
                   initAudio.initProcessor(std::unique_ptr<Processor>(new PitchProcessor(initPitch)))),
            pitch(initPitch)
        {
        }

        Pitch::~Pitch()
        {
        }

        void Pitch::setPitch(float newPitch)
        {
            pitch = newPitch;

            audio.updateProcessor(processorId, [newPitch](Object* node) {
                PitchProcessor* pitchProcessor = static_cast<PitchProcessor*>(node);
                pitchProcessor->setPitch(newPitch);
            });
        }
    } // namespace audio
} // namespace ouzel
