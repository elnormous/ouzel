// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
        Delay::Delay(Audio& initAudio, float initDelay):
            Effect(initAudio),
            delay(initDelay)
        {
        }

        Delay::~Delay()
        {
        }

        void Delay::process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                            std::vector<float>& samples)
        {
            const auto delayFrames = static_cast<uint32_t>(delay * sampleRate);
            const auto bufferFrames = frames + delayFrames;

            buffer.resize(bufferFrames * channels);

            for (uint16_t channel = 0; channel < channels; ++channel)
            {
                float* bufferChannel = &buffer[channel * bufferFrames];
                float* outputChannel = &samples[channel * frames];

                for (uint32_t frame = 0; frame < frames; ++frame)
                    bufferChannel[frame + delayFrames] += outputChannel[frame];

                    for (uint32_t frame = 0; frame < frames; ++frame)
                        outputChannel[frame] = bufferChannel[frame];

                        // erase frames from beginning
                        for (uint32_t frame = 0; frame < bufferFrames - frames; ++frame)
                            bufferChannel[frame] = bufferChannel[frame + frames];

                            for (uint32_t frame = bufferFrames - frames; frame < bufferFrames; ++frame)
                                bufferChannel[frame] = 0.0F;
            }
        }

        void Delay::setDelay(float newDelay)
        {
            delay = newDelay;
        }

        void Delay::setDelayRandom(const std::pair<float, float>& newDelayRandom)
        {
            delayRandom = newDelayRandom;
            // TODO: pass to processor
        }

        Gain::Gain(Audio& initAudio, float initGain):
            Effect(initAudio),
            gain(initGain)
        {
        }

        Gain::~Gain()
        {
        }

        void Gain::process(uint32_t, uint16_t, uint32_t,
                     std::vector<float>& samples)
        {
            for (float& sample : samples)
                sample *= gainFactor;
        }

        void Gain::setGain(float newGain)
        {
            gain = newGain;
            gainFactor = pow(10.0F, gain / 20.0F);
        }

        void Gain::setGainRandom(const std::pair<float, float>& newGainRandom)
        {
            gainRandom = newGainRandom;
            // TODO: pass to processor
        }

        Panner::Panner(Audio& initAudio):
            Effect(initAudio)
        {
        }

        Panner::~Panner()
        {
        }

        void Panner::process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                             std::vector<float>& samples)
        {
        }

        void Panner::setPosition(const Vector3F& newPosition)
        {
            position = newPosition;
        }

        void Panner::setRolloffFactor(float newRolloffFactor)
        {
            rolloffFactor = newRolloffFactor;
        }

        void Panner::setMinDistance(float newMinDistance)
        {
            minDistance = newMinDistance;
        }

        void Panner::setMaxDistance(float newMaxDistance)
        {
            maxDistance = newMaxDistance;
        }

        void Panner::updateTransform()
        {
            setPosition(actor->getWorldPosition());
        }

        namespace
        {
            constexpr float MIN_PITCH = 0.5F;
            constexpr float MAX_PITCH = 2.0F;
        }

        PitchScale::PitchScale(Audio& initAudio, float initScale):
            Effect(initAudio),
            scale(initScale)
        {
        }

        PitchScale::~PitchScale()
        {
        }

        void PitchScale::process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                                 std::vector<float>& samples)
        {
            pitchShift.resize(channels);

            for (uint16_t channel = 0; channel < channels; ++channel)
                pitchShift[channel].process(scale, frames, sampleRate,
                                            &samples[channel * frames],
                                            &samples[channel * frames]);
        }

        void PitchScale::setScale(float newScale)
        {
            scale = clamp(newScale, MIN_PITCH, MAX_PITCH);
        }

        void PitchScale::setScaleRandom(const std::pair<float, float>& newScaleRandom)
        {
            scaleRandom = newScaleRandom;
            // TODO: pass to processor
        }

        PitchShift::PitchShift(Audio& initAudio, float initShift):
            Effect(initAudio),
            shift(initShift)
        {
        }

        PitchShift::~PitchShift()
        {
        }

        void PitchShift::process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                                 std::vector<float>& samples)
        {
            // TODO: implement
        }

        void PitchShift::setShift(float newShift)
        {
            shift = newShift;
        }

        void PitchShift::setShiftRandom(const std::pair<float, float>& newShiftRandom)
        {
            shiftRandom = newShiftRandom;
            // TODO: pass to processor
        }

        Reverb::Reverb(Audio& initAudio, float initDelay, float initDecay):
            Effect(initAudio),
            delay(initDelay),
            decay(initDecay)
        {
        }

        Reverb::~Reverb()
        {
        }

        void Reverb::process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                             std::vector<float>& samples)
        {
            const auto delayFrames = static_cast<uint32_t>(delay * sampleRate);
            const auto bufferFrames = frames + delayFrames;

            buffers.resize(channels);

            for (uint16_t channel = 0; channel < channels; ++channel)
            {
                std::vector<float>& buffer = buffers[channel];
                buffer.resize(bufferFrames);

                float* outputChannel = &samples[channel * frames];

                for (uint32_t frame = 0; frame < frames; ++frame)
                    buffer[frame] += outputChannel[frame];

                    for (uint32_t frame = 0; frame < frames; ++frame)
                        buffer[frame + delayFrames] += buffer[frame] * decay;

                        for (uint32_t frame = 0; frame < frames; ++frame)
                            outputChannel[frame] = buffer[frame];

                            // erase frames from beginning
                            buffer.erase(buffer.begin(), buffer.begin() + frames);
            }
        }

        LowPass::LowPass(Audio& initAudio):
            Effect(initAudio)
        {
        }

        LowPass::~LowPass()
        {
        }

        void LowPass::process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                              std::vector<float>& samples)
        {
        }

        HighPass::HighPass(Audio& initAudio):
            Effect(initAudio)
        {
        }

        HighPass::~HighPass()
        {
        }

        void HighPass::process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                               std::vector<float>& samples)
        {
        }
    } // namespace audio
} // namespace ouzel
