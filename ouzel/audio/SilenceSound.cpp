// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "SilenceSound.hpp"
#include "Audio.hpp"
#include "mixer/Source.hpp"
#include "mixer/SourceData.hpp"

namespace ouzel
{
    namespace audio
    {
        class SilenceData;

        class SilenceSource: public Source
        {
        public:
            SilenceSource(SilenceData& toneData);

            void reset() override
            {
                position = 0;
            }

            void getData(uint32_t frames, std::vector<float>& samples) override;

        private:
            uint32_t position = 0;
        };

        class SilenceData: public SourceData
        {
        public:
            SilenceData(float initLength):
                length(initLength)
            {
                channels = 1;
                sampleRate = 44100;
            }

            float getLength() const { return length; }

            std::unique_ptr<Source> createSource() override
            {
                return std::unique_ptr<Source>(new SilenceSource(*this));
            }

        private:
            float length;
        };

        SilenceSource::SilenceSource(SilenceData& silenceData):
            Source(silenceData)
        {
        }

        void SilenceSource::getData(uint32_t frames, std::vector<float>& samples)
        {
            SilenceData& silenceData = static_cast<SilenceData&>(sourceData);

            samples.resize(frames);
            std::fill(samples.begin(), samples.end(), 0.0F);

            const uint32_t sampleRate = sourceData.getSampleRate();
            const float length = static_cast<SilenceData&>(silenceData).getLength();

            if (length > 0.0F)
            {
                const uint32_t frameCount = static_cast<uint32_t>(length * sampleRate);
                uint32_t neededSize = frames;

                while (neededSize > 0)
                {
                    if (isRepeating() && (frameCount - position) == 0) reset();

                    if (frameCount - position < neededSize)
                    {
                        neededSize -= frameCount - position;
                        position = frameCount;
                    }
                    else
                    {
                        position += neededSize;
                        neededSize = 0;
                    }

                    if (!isRepeating()) break;
                }

                if ((frameCount - position) == 0)
                {
                    if (!isRepeating()) playing = false; // TODO: fire event
                    reset();
                }
            }
            else
            {
                position += frames;
            }
        }

        SilenceSound::SilenceSound(Audio& initAudio, float initLength):
            Sound(initAudio, initAudio.initSourceData(std::unique_ptr<SourceData>(new SilenceData(initLength)))),
            length(initLength)
        {
        }
    } // namespace audio
} // namespace ouzel
