// Ouzel by Elviss Strazdins

#include <algorithm>
#include <cmath>
#include "Effects.hpp"
#include "Audio.hpp"
#include "../scene/Actor.hpp"
#include "../math/MathUtils.hpp"
#include "smbPitchShift.hpp"

namespace ouzel::audio
{
    class DelayProcessor final: public mixer::Processor
    {
    public:
        explicit DelayProcessor(float initDelay):
            delay(initDelay)
        {
        }

        void process(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate,
                     std::vector<float>& samples) final
        {
            const auto delayFrames = static_cast<std::uint32_t>(delay * sampleRate);
            const auto bufferFrames = frames + delayFrames;

            buffer.resize(bufferFrames * channels);

            for (std::uint32_t channel = 0; channel < channels; ++channel)
            {
                float* bufferChannel = &buffer[channel * bufferFrames];
                float* outputChannel = &samples[channel * frames];

                for (std::uint32_t frame = 0; frame < frames; ++frame)
                    bufferChannel[frame + delayFrames] += outputChannel[frame];

                for (std::uint32_t frame = 0; frame < frames; ++frame)
                    outputChannel[frame] = bufferChannel[frame];

                // erase frames from beginning
                for (std::uint32_t frame = 0; frame < bufferFrames - frames; ++frame)
                    bufferChannel[frame] = bufferChannel[frame + frames];

                for (std::uint32_t frame = bufferFrames - frames; frame < bufferFrames; ++frame)
                    bufferChannel[frame] = 0.0F;
            }
        }

        void setDelay(float newDelay)
        {
            delay = newDelay;
        }

    private:
        float delay = 0.0F;
        std::vector<float> buffer;
    };

    Delay::Delay(Audio& initAudio, float initDelay):
        Effect(initAudio,
               initAudio.initProcessor(std::make_unique<DelayProcessor>(initDelay))),
        delay(initDelay)
    {
    }

    void Delay::setDelay(float newDelay)
    {
        delay = newDelay;

        audio.updateProcessor(processorId, [newDelay](mixer::Object* node) {
            auto delayProcessor = static_cast<DelayProcessor*>(node);
            delayProcessor->setDelay(newDelay);
        });
    }

    void Delay::setDelayRandom(const std::pair<float, float>& newDelayRandom)
    {
        delayRandom = newDelayRandom;
        // TODO: pass to processor
    }

    class GainProcessor final: public mixer::Processor
    {
    public:
        explicit GainProcessor(float initGain = 0.0F):
            gain(initGain),
            gainFactor(std::pow(10.0F, initGain / 20.0F))
        {
        }

        void process(std::uint32_t, std::uint32_t, std::uint32_t,
                     std::vector<float>& samples) final
        {
            for (float& sample : samples)
                sample *= gainFactor;
        }

        void setGain(float newGain)
        {
            gain = newGain;
            gainFactor = std::pow(10.0F, gain / 20.0F);
        }

    private:
        float gain = 0.0F;
        float gainFactor = 1.0F;
    };

    Gain::Gain(Audio& initAudio, float initGain):
        Effect(initAudio,
               initAudio.initProcessor(std::make_unique<GainProcessor>(initGain))),
        gain(initGain)
    {
    }

    void Gain::setGain(float newGain)
    {
        gain = newGain;

        audio.updateProcessor(processorId, [newGain](mixer::Object* node) {
            auto gainProcessor = static_cast<GainProcessor*>(node);
            gainProcessor->setGain(newGain);
        });
    }

    void Gain::setGainRandom(const std::pair<float, float>& newGainRandom)
    {
        gainRandom = newGainRandom;
        // TODO: pass to processor
    }

    class PannerProcessor final: public mixer::Processor
    {
    public:
        PannerProcessor()
        {
        }

        void process(std::uint32_t, std::uint32_t, std::uint32_t,
                     std::vector<float>&) final
        {
        }

        void setPosition(const Vector<float, 3>& newPosition)
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
        Vector<float, 3> position;
        float rolloffFactor = 1.0F;
        float minDistance = 1.0F;
        float maxDistance = FLT_MAX;
    };

    Panner::Panner(Audio& initAudio):
        Effect(initAudio,
               initAudio.initProcessor(std::make_unique<PannerProcessor>()))
    {
    }

    void Panner::setPosition(const Vector<float, 3>& newPosition)
    {
        position = newPosition;

        audio.updateProcessor(processorId, [newPosition](mixer::Object* node) {
            auto pannerProcessor = static_cast<PannerProcessor*>(node);
            pannerProcessor->setPosition(newPosition);
        });
    }

    void Panner::setRolloffFactor(float newRolloffFactor)
    {
        rolloffFactor = newRolloffFactor;

        audio.updateProcessor(processorId, [newRolloffFactor](mixer::Object* node) {
            auto pannerProcessor = static_cast<PannerProcessor*>(node);
            pannerProcessor->setRolloffFactor(newRolloffFactor);
        });
    }

    void Panner::setMinDistance(float newMinDistance)
    {
        minDistance = newMinDistance;

        audio.updateProcessor(processorId, [newMinDistance](mixer::Object* node) {
            auto pannerProcessor = static_cast<PannerProcessor*>(node);
            pannerProcessor->setMinDistance(newMinDistance);
        });
    }

    void Panner::setMaxDistance(float newMaxDistance)
    {
        maxDistance = newMaxDistance;

        audio.updateProcessor(processorId, [newMaxDistance](mixer::Object* node) {
            auto pannerProcessor = static_cast<PannerProcessor*>(node);
            pannerProcessor->setMaxDistance(newMaxDistance);
        });
    }

    void Panner::updateTransform()
    {
        setPosition(actor->getWorldPosition());
    }

    namespace
    {
        constexpr float minPitch = 0.5F;
        constexpr float maxPitch = 2.0F;
    }

    class PitchScaleProcessor final: public mixer::Processor
    {
    public:
        explicit PitchScaleProcessor(float initScale):
            scale(std::clamp(initScale, minPitch, maxPitch))
        {
        }

        void process(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate,
                     std::vector<float>& samples) final
        {
            pitchShift.resize(channels);

            for (std::uint32_t channel = 0; channel < channels; ++channel)
                pitchShift[channel].process(scale, frames, sampleRate,
                                            &samples[channel * frames],
                                            &samples[channel * frames]);
        }

        void setScale(float newScale)
        {
            scale = std::clamp(newScale, minPitch, maxPitch);
        }

    private:
        float scale = 1.0f;
        std::vector<smb::PitchShift<1024, 4>> pitchShift;
    };

    PitchScale::PitchScale(Audio& initAudio, float initScale):
        Effect(initAudio,
               initAudio.initProcessor(std::make_unique<PitchScaleProcessor>(initScale))),
        scale(initScale)
    {
    }

    void PitchScale::setScale(float newScale)
    {
        scale = newScale;

        audio.updateProcessor(processorId, [newScale](mixer::Object* node) {
            auto pitchScaleProcessor = static_cast<PitchScaleProcessor*>(node);
            pitchScaleProcessor->setScale(newScale);
        });
    }

    void PitchScale::setScaleRandom(const std::pair<float, float>& newScaleRandom)
    {
        scaleRandom = newScaleRandom;
        // TODO: pass to processor
    }

    class PitchShiftProcessor final: public mixer::Processor
    {
    public:
        explicit PitchShiftProcessor(float initShift):
            shift(initShift)
        {
        }

        void process(std::uint32_t, std::uint32_t, std::uint32_t,
                     std::vector<float>&) final
        {
            // TODO: implement
        }

        void setShift(float newShift)
        {
            shift = newShift;
        }

    private:
        float shift = 1.0f;
    };

    PitchShift::PitchShift(Audio& initAudio, float initShift):
        Effect(initAudio,
               initAudio.initProcessor(std::make_unique<PitchShiftProcessor>(initShift))),
        shift(initShift)
    {
    }

    void PitchShift::setShift(float newShift)
    {
        shift = newShift;

        audio.updateProcessor(processorId, [newShift](mixer::Object* node) {
            auto pitchShiftProcessor = static_cast<PitchShiftProcessor*>(node);
            pitchShiftProcessor->setShift(newShift);
        });
    }

    void PitchShift::setShiftRandom(const std::pair<float, float>& newShiftRandom)
    {
        shiftRandom = newShiftRandom;
        // TODO: pass to processor
    }

    class ReverbProcessor final: public mixer::Processor
    {
    public:
        ReverbProcessor(float initDelay, float initDecay):
            delay(initDelay), decay(initDecay)
        {
        }

        void process(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate,
                     std::vector<float>& samples) final
        {
            const auto delayFrames = static_cast<std::uint32_t>(delay * sampleRate);
            const auto bufferFrames = frames + delayFrames;

            buffers.resize(channels);

            for (std::uint32_t channel = 0; channel < channels; ++channel)
            {
                std::vector<float>& buffer = buffers[channel];
                buffer.resize(bufferFrames);

                float* outputChannel = &samples[channel * frames];

                for (std::uint32_t frame = 0; frame < frames; ++frame)
                    buffer[frame] += outputChannel[frame];

                for (std::uint32_t frame = 0; frame < frames; ++frame)
                    buffer[frame + delayFrames] += buffer[frame] * decay;

                for (std::uint32_t frame = 0; frame < frames; ++frame)
                    outputChannel[frame] = buffer[frame];

                // erase frames from beginning
                buffer.erase(buffer.begin(), buffer.begin() + frames);
            }
        }

    private:
        float delay = 0.1F;
        float decay = 0.5F;
        std::vector<std::vector<float>> buffers;
    };

    Reverb::Reverb(Audio& initAudio, float initDelay, float initDecay):
        Effect(initAudio,
               initAudio.initProcessor(std::make_unique<ReverbProcessor>(initDelay, initDecay))),
        delay(initDelay),
        decay(initDecay)
    {
    }

    class LowPassProcessor final: public mixer::Processor
    {
    public:
        void process(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate,
                     std::vector<float>& samples) final
        {
        }
    };

    LowPass::LowPass(Audio& initAudio):
        Effect(initAudio,
               initAudio.initProcessor(std::unique_ptr<LowPassProcessor>()))
    {
    }

    class HighPassProcessor final: public mixer::Processor
    {
    public:
        void process(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate,
                     std::vector<float>& samples) final
        {
        }
    };

    HighPass::HighPass(Audio& initAudio):
        Effect(initAudio,
               initAudio.initProcessor(std::unique_ptr<HighPassProcessor>()))
    {
    }
}
