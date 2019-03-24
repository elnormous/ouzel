// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <iterator>
#include <stdexcept>
#include "PcmSound.hpp"
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

            uint32_t totalSize = 0;

            if (neededSize > 0)
            {
                if (data.size() - position < neededSize)
                {
                    std::copy(data.begin() + position,
                              data.end(),
                              samples.begin() + totalSize);
                    totalSize += static_cast<uint32_t>(data.size() - position);
                    neededSize -= static_cast<uint32_t>(data.size() - position);
                    position = static_cast<uint32_t>(data.size());
                }
                else
                {
                    std::copy(data.begin() + position,
                              data.begin() + position + neededSize,
                              samples.begin() + totalSize);
                    totalSize += neededSize;
                    position += neededSize;
                    neededSize = 0;
                }
            }

            if ((data.size() - position) == 0)
            {
                playing = false; // TODO: fire event
                reset();
            }

            std::fill(samples.begin() + totalSize, samples.end(), 0.0F); // TODO: remove
        }

        PcmSound::PcmSound(Audio& initAudio, uint16_t channels, uint32_t sampleRate,
                           const std::vector<float>& samples):
            Sound(initAudio, initAudio.initSource([channels, sampleRate, samples](){
                return std::unique_ptr<mixer::Source>(new PcmData(channels, sampleRate, samples));
            }))
        {
        }
    } // namespace audio
} // namespace ouzel
