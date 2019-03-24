// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "SilenceSound.hpp"
#include "Audio.hpp"
#include "mixer/Stream.hpp"
#include "mixer/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        class SilenceData;

        class SilenceSource final: public mixer::Stream
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

        class SilenceData final: public mixer::Source
        {
        public:
            SilenceData(float initLength):
                length(initLength)
            {
                channels = 1;
                sampleRate = 44100;
            }

            float getLength() const { return length; }

            std::unique_ptr<mixer::Stream> createStream() override
            {
                return std::unique_ptr<mixer::Stream>(new SilenceSource(*this));
            }

        private:
            float length;
        };

        SilenceSource::SilenceSource(SilenceData& silenceData):
            Stream(silenceData)
        {
        }

        void SilenceSource::getData(uint32_t frames, std::vector<float>& samples)
        {
            SilenceData& silenceData = static_cast<SilenceData&>(source);

            samples.resize(frames);
            std::fill(samples.begin(), samples.end(), 0.0F); // TODO: fill only the needed samples

            const uint32_t sampleRate = source.getSampleRate();
            const float length = static_cast<SilenceData&>(silenceData).getLength();

            if (length > 0.0F)
            {
                const uint32_t frameCount = static_cast<uint32_t>(length * sampleRate);
                uint32_t neededSize = frames;

                if (neededSize > 0)
                {
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
                }

                if ((frameCount - position) == 0)
                {
                    playing = false; // TODO: fire event
                    reset();
                }
            }
            else
            {
                position += frames;
            }
        }

        SilenceSound::SilenceSound(Audio& initAudio, float initLength):
            Sound(initAudio, initAudio.initSource([initLength](){
                return std::unique_ptr<mixer::Source>(new SilenceData(initLength));
            })),
            length(initLength)
        {
        }
    } // namespace audio
} // namespace ouzel
