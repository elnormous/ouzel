// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <iterator>
#include <stdexcept>
#include "PCMSound.hpp"
#include "Audio.hpp"
#include "mixer/Source.hpp"
#include "mixer/SourceData.hpp"

namespace ouzel
{
    namespace audio
    {
        class PCMData;

        class PCMSource: public Source
        {
        public:
            PCMSource(PCMData& pcmData);

            void reset() override
            {
                offset = 0;
            }

        private:
            void getData(std::vector<float>& samples) override
            {
                uint32_t neededSize = static_cast<uint32_t>(samples.size());
                uint32_t totalSize = 0;

                while (neededSize > 0)
                {
                    if (isRepeating() && (data.size() - offset) == 0) reset();

                    if (data.size() - offset < neededSize)
                    {
                        std::copy(data.begin() + offset,
                                  data.end(),
                                  samples.begin() + totalSize);
                        totalSize += static_cast<uint32_t>(data.size() - offset);
                        neededSize -= static_cast<uint32_t>(data.size() - offset);
                        offset = static_cast<uint32_t>(data.size());
                    }
                    else
                    {
                        std::copy(data.begin() + offset,
                                  data.begin() + offset + neededSize,
                                  samples.begin() + totalSize);
                        totalSize += neededSize;
                        offset += neededSize;
                        neededSize = 0;
                    }

                    if (!isRepeating()) break;
                }

                if ((data.size() - offset) == 0) reset();

                std::fill(samples.begin() + totalSize, samples.end(), 0.0F);
            }

            const std::vector<float>& data;
            uint32_t offset = 0;
        };

        class PCMData: public SourceData
        {
        public:
            PCMData(uint16_t initChannels, uint32_t initSampleRate,
                    const std::vector<float>& initSamples):
                samples(initSamples)
            {
                channels = initChannels;
                sampleRate = initSampleRate;
            }

            const std::vector<float>& getSamples() const { return samples; }

            std::unique_ptr<Source> createSource() override
            {
                return std::unique_ptr<Source>(new PCMSource(*this));
            }

        private:
            std::vector<float> samples;
        };

        PCMSource::PCMSource(PCMData& pcmData):
            Source(pcmData),
            data(pcmData.getSamples())
        {
        }

        PCMSound::PCMSound(Audio& initAudio, uint16_t channels, uint32_t sampleRate,
                           const std::vector<float>& samples):
            Sound(initAudio, initAudio.initSourceData(std::unique_ptr<SourceData>(new PCMData(channels, sampleRate, samples))))
        {
        }
    } // namespace audio
} // namespace ouzel
