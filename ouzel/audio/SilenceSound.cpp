// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "SilenceSound.hpp"
#include "Audio.hpp"
#include "mixer/Data.hpp"
#include "mixer/Stream.hpp"

namespace ouzel
{
    namespace audio
    {
        class SilenceData;

        class SilenceStream final: public mixer::Stream
        {
        public:
            SilenceStream(SilenceData& toneData);

            void reset() override
            {
                position = 0;
            }

            void getSamples(uint32_t frames, std::vector<float>& samples) override;

        private:
            uint32_t position = 0;
        };

        class SilenceData final: public mixer::Data
        {
        public:
            SilenceData(float initLength):
                length(initLength)
            {
                channels = 1;
                sampleRate = 44100;
            }

            inline float getLength() const { return length; }

            std::unique_ptr<mixer::Stream> createStream() override
            {
                return std::unique_ptr<mixer::Stream>(new SilenceStream(*this));
            }

        private:
            float length;
        };

        SilenceStream::SilenceStream(SilenceData& silenceData):
            Stream(silenceData)
        {
        }

        void SilenceStream::getSamples(uint32_t frames, std::vector<float>& samples)
        {
            SilenceData& silenceData = static_cast<SilenceData&>(data);

            samples.resize(frames);
            std::fill(samples.begin(), samples.end(), 0.0F); // TODO: fill only the needed samples

            const float length = static_cast<SilenceData&>(silenceData).getLength();

            if (length > 0.0F)
            {
                const uint32_t frameCount = static_cast<uint32_t>(length * data.getSampleRate());
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
            Sound(initAudio, initAudio.initData([initLength](){
                return std::unique_ptr<mixer::Data>(new SilenceData(initLength));
            })),
            length(initLength)
        {
        }
    } // namespace audio
} // namespace ouzel
