// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <iterator>
#include <stdexcept>
#include "PcmClip.hpp"
#include "Audio.hpp"
#include "mixer/Stream.hpp"
#include "mixer/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        class PcmData;

        class PcmSource final: public mixer::Stream
        {
        public:
            PcmSource(PcmData& pcmData);

            void reset() override
            {
                position = 0;
            }

            void getData(uint32_t frames, std::vector<float>& samples) override;

        private:
            uint32_t position = 0;
        };

        class PcmData final: public mixer::Source
        {
        public:
            PcmData(uint16_t initChannels, uint32_t initSampleRate,
                    const std::vector<float>& initSamples):
                samples(initSamples)
            {
                channels = initChannels;
                sampleRate = initSampleRate;
            }

            const std::vector<float>& getSamples() const { return samples; }

            std::unique_ptr<mixer::Stream> createStream() override
            {
                return std::unique_ptr<mixer::Stream>(new PcmSource(*this));
            }

        private:
            std::vector<float> samples;
        };

        PcmSource::PcmSource(PcmData& pcmData):
            Stream(pcmData)
        {
        }

        void PcmSource::getData(uint32_t frames, std::vector<float>& samples)
        {
            uint32_t neededSize = frames * source.getChannels();
            samples.resize(neededSize);

            PcmData& pcmData = static_cast<PcmData&>(source);
            const std::vector<float>& data = pcmData.getSamples();

            uint32_t sourceFrames = static_cast<uint32_t>(data.size() / pcmData.getChannels());

            uint32_t copyFrames = frames;
            if (copyFrames > sourceFrames - position)
                copyFrames = sourceFrames - position;

            for (uint32_t channel = 0; channel < pcmData.getChannels(); ++channel)
            {
                const float* sourceChannel = &data[channel * sourceFrames];
                float* outputChannel = &samples[channel * frames];

                for (uint32_t frame = 0; frame < copyFrames; ++frame)
                    outputChannel[frame] = sourceChannel[frame + position];
            }

            position += copyFrames;

            for (uint32_t channel = 0; channel < pcmData.getChannels(); ++channel)
            {
                float* outputChannel = &samples[channel * frames];

                for (uint32_t frame = copyFrames; frame < frames; ++frame)
                    outputChannel[frame] = 0.0F;
            }

            if ((sourceFrames - position) == 0)
            {
                playing = false; // TODO: fire event
                reset();
            }
        }

        PcmClip::PcmClip(Audio& initAudio, uint16_t channels, uint32_t sampleRate,
                          const std::vector<float>& samples):
            Sound(initAudio, initAudio.initSource([channels, sampleRate, samples](){
                return std::unique_ptr<mixer::Source>(new PcmData(channels, sampleRate, samples));
            }))
        {
        }
    } // namespace audio
} // namespace ouzel
