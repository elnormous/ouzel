// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cmath>
#include "Effects.hpp"
#include "Audio.hpp"
#include "scene/Actor.hpp"
#include "math/MathUtils.hpp"
#include "smbPitchShift.hpp"

namespace ouzel
{
    namespace audio
    {
        class DelayProcessor final: public mixer::Processor
        {
        public:
            DelayProcessor()
            {
            }

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override
            {
            }
        };

        Delay::Delay(Audio& initAudio):
            Effect(initAudio,
                   initAudio.initProcessor(std::unique_ptr<mixer::Processor>(new DelayProcessor())))
        {
        }

        Delay::~Delay()
        {
        }

        void Delay::setDelay(float newDelay)
        {
            delay = newDelay;
        }

        class GainProcessor final: public mixer::Processor
        {
        public:
            GainProcessor()
            {
            }

            void process(uint32_t, uint16_t, uint32_t,
                         std::vector<float>& samples) override
            {
                for (float& sample : samples)
                    sample *= gainFactor;
            }

            void setGain(float newGain)
            {
                gain = newGain;
                gainFactor = powf(10.0F, gain / 20.0F);
            }

        private:
            float gain = 0.0F;
            float gainFactor = 1.0F;
        };

        Gain::Gain(Audio& initAudio):
            Effect(initAudio,
                   initAudio.initProcessor(std::unique_ptr<mixer::Processor>(new GainProcessor())))
        {
        }

        Gain::~Gain()
        {
        }

        void Gain::setGain(float newGain)
        {
            gain = newGain;

            audio.updateProcessor(processorId, [newGain](mixer::Object* node) {
                GainProcessor* gainProcessor = static_cast<GainProcessor*>(node);
                gainProcessor->setGain(newGain);
            });
        }

        class PannerProcessor final: public mixer::Processor
        {
        public:
            PannerProcessor()
            {
            }

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override
            {
            }

            void setPosition(const Vector3<float>& newPosition)
            {
                position = newPosition;
            }

            void setRolloffFactor(float newRolloffFactor)
            {
                rolloffFactor = newRolloffFactor;
            }

            void setMinDistance(float newMinDistance)
            {
                minDistance = newMinDistance;
            }

            void setMaxDistance(float newMaxDistance)
            {
                maxDistance = newMaxDistance;
            }

        private:
            Vector3<float> position;
            float rolloffFactor = 1.0F;
            float minDistance = 1.0F;
            float maxDistance = FLT_MAX;
        };

        Panner::Panner(Audio& initAudio):
            Effect(initAudio,
                   initAudio.initProcessor(std::unique_ptr<mixer::Processor>(new PannerProcessor()))),
            scene::Component(scene::Component::SOUND)
        {
        }

        Panner::~Panner()
        {
        }

        void Panner::setPosition(const Vector3<float>& newPosition)
        {
            position = newPosition;

            audio.updateProcessor(processorId, [newPosition](mixer::Object* node) {
                PannerProcessor* pannerProcessor = static_cast<PannerProcessor*>(node);
                pannerProcessor->setPosition(newPosition);
            });
        }

        void Panner::setRolloffFactor(float newRolloffFactor)
        {
            rolloffFactor = newRolloffFactor;

            audio.updateProcessor(processorId, [newRolloffFactor](mixer::Object* node) {
                PannerProcessor* pannerProcessor = static_cast<PannerProcessor*>(node);
                pannerProcessor->setRolloffFactor(newRolloffFactor);
            });
        }

        void Panner::setMinDistance(float newMinDistance)
        {
            minDistance = newMinDistance;

            audio.updateProcessor(processorId, [newMinDistance](mixer::Object* node) {
                PannerProcessor* pannerProcessor = static_cast<PannerProcessor*>(node);
                pannerProcessor->setMinDistance(newMinDistance);
            });
        }

        void Panner::setMaxDistance(float newMaxDistance)
        {
            maxDistance = newMaxDistance;

            audio.updateProcessor(processorId, [newMaxDistance](mixer::Object* node) {
                PannerProcessor* pannerProcessor = static_cast<PannerProcessor*>(node);
                pannerProcessor->setMaxDistance(newMaxDistance);
            });
        }

        void Panner::updateTransform()
        {
            setPosition(actor->getWorldPosition());
        }

        static constexpr float MIN_PITCH = 0.5F;
        static constexpr float MAX_PITCH = 2.0F;

        class PitchProcessor final: public mixer::Processor
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
            Effect(initAudio,
                   initAudio.initProcessor(std::unique_ptr<mixer::Processor>(new PitchProcessor(initPitch)))),
            pitch(initPitch)
        {
        }

        Pitch::~Pitch()
        {
        }

        void Pitch::setPitch(float newPitch)
        {
            pitch = newPitch;

            audio.updateProcessor(processorId, [newPitch](mixer::Object* node) {
                PitchProcessor* pitchProcessor = static_cast<PitchProcessor*>(node);
                pitchProcessor->setPitch(newPitch);
            });
        }

        class ReverbProcessor final: public mixer::Processor
        {
        public:
            ReverbProcessor(float initDelay, float initDecay):
                delay(initDelay), decay(initDecay)
            {
            }

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override
            {
                uint32_t delaySamples = static_cast<uint32_t>(delay * sampleRate);

                if (buffer.size() < (frames + delaySamples) * channels)
                    buffer.resize((frames + delaySamples) * channels);

                for (uint32_t frame = 0; frame < frames; ++frame)
                    for (uint16_t channel = 0; channel < channels; ++channel)
                        buffer[frame * channels + channel] += samples[frame * channels + channel];

                for (uint32_t frame = 0; frame < frames; ++frame)
                    for (uint16_t channel = 0; channel < channels; ++channel)
                        buffer[(frame + delaySamples) * channels + channel] += buffer[frame * channels + channel] * decay;

                for (uint32_t frame = 0; frame < frames; ++frame)
                    for (uint16_t channel = 0; channel < channels; ++channel)
                        samples[frame * channels + channel] = buffer[frame * channels + channel];

                buffer.erase(buffer.begin(), buffer.begin() + frames * channels);
            }

        private:
            std::vector<float> buffer;
            float delay = 0.1F;
            float decay = 0.5F;
        };

        Reverb::Reverb(Audio& initAudio, float initDelay, float initDecay):
            Effect(initAudio,
                   initAudio.initProcessor(std::unique_ptr<mixer::Processor>(new ReverbProcessor(initDelay, initDecay))))
        {
        }

        Reverb::~Reverb()
        {
        }
    } // namespace audio
} // namespace ouzel
