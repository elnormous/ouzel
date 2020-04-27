// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
            explicit SilenceStream(SilenceData& toneData);

            void reset() final
            {
                position = 0;
            }

            void getSamples(std::uint32_t frames, std::vector<float>& samples) final;

        private:
            std::uint32_t position = 0;
        };

        class SilenceData final: public mixer::Data
        {
        public:
            explicit SilenceData(float initLength):
                length(initLength)
            {
                channels = 1;
                sampleRate = 44100;
            }

            auto getLength() const noexcept { return length; }

            std::unique_ptr<mixer::Stream> createStream() final
            {
                return std::make_unique<SilenceStream>(*this);
            }

        private:
            float length;
        };

        SilenceStream::SilenceStream(SilenceData& silenceData):
            Stream(silenceData)
        {
        }

        void SilenceStream::getSamples(std::uint32_t frames, std::vector<float>& samples)
        {
            SilenceData& silenceData = static_cast<SilenceData&>(data);

            samples.resize(frames);
            std::fill(samples.begin(), samples.end(), 0.0F); // TODO: fill only the needed samples

            const auto length = static_cast<SilenceData&>(silenceData).getLength();

            if (length > 0.0F)
            {
                const auto frameCount = static_cast<std::uint32_t>(length * data.getSampleRate());
                auto neededSize = frames;

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
            Sound(initAudio,
                  initAudio.initData(std::unique_ptr<mixer::Data>(data = new SilenceData(initLength))),
                  Sound::Format::Pcm),
            length(initLength)
        {
        }
    } // namespace audio
} // namespace ouzel
