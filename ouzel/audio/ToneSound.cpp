// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "ToneSound.hpp"
#include "Audio.hpp"
#include "mixer/Source.hpp"
#include "mixer/SourceData.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        class ToneData;

        class ToneSource: public Source
        {
        public:
            ToneSource(ToneData& toneData);

            void reset() override
            {
                offset = 0;
            }

            void getData(uint32_t frames, std::vector<float>& samples) override;

        private:
            uint32_t offset = 0;
        };

        class ToneData: public SourceData
        {
        public:
            ToneData(float initFrequency, ToneSound::Type initType, float initAmplitude, float initLength):
                frequency(initFrequency),
                type(initType),
                amplitude(initAmplitude),
                length(initLength)
            {
                channels = 1;
                sampleRate = 44100;
            }

            float getFrequency() const { return frequency; }
            ToneSound::Type getType() const { return type; }
            float getAmplitude() const { return amplitude; }
            float getLength() const { return length; }

            std::unique_ptr<Source> createSource() override
            {
                return std::unique_ptr<Source>(new ToneSource(*this));
            }

        private:
            float frequency;
            ToneSound::Type type;
            float amplitude;
            float length;
        };

        ToneSource::ToneSource(ToneData& toneData):
            Source(toneData)
        {
        }

        static void generateWave(ToneSound::Type type, uint32_t frames, uint32_t offset,
                                 float frameLength, float amplitude, std::vector<float>& samples)
        {
            for (uint32_t i = 0; i < frames; ++i)
            {
                float t = static_cast<float>(offset) * frameLength;

                switch (type)
                {
                    case ToneSound::Type::SINE:
                        samples[i] = sinf(t * TAU);
                        break;
                    case ToneSound::Type::SQUARE:
                        samples[i] = fmodf(roundf(t * 2.0F + 0.5F), 2.0F) * 2.0F - 1.0F;
                        break;
                    case ToneSound::Type::SAWTOOTH:
                        samples[i] = fmodf(t + 0.5F, 1.0F) * 2.0F - 1.0F;
                        break;
                    case ToneSound::Type::TRIANGLE:
                        samples[i] = fabsf(fmodf(t + 0.75F, 1.0F) * 2.0F - 1.0F) * 2.0F - 1.0F;
                        break;
                }

                samples[i] *= amplitude;

                ++offset;
            }
        }

        void ToneSource::getData(uint32_t frames, std::vector<float>& samples)
        {
            ToneData& toneData = static_cast<ToneData&>(sourceData);

            samples.resize(frames);

            const uint32_t sampleRate = sourceData.getSampleRate();
            const float length = static_cast<ToneData&>(sourceData).getLength();

            if (length > 0.0F)
            {
                const uint32_t frameCount = static_cast<uint32_t>(length * sampleRate);
                uint32_t neededSize = frames;
                uint32_t totalSize = 0;

                while (neededSize > 0)
                {
                    if (isRepeating() && (frameCount - offset) == 0) reset();

                    if (frameCount - offset < neededSize)
                    {
                        generateWave(toneData.getType(), frameCount - offset, offset,
                                     toneData.getFrequency() / static_cast<float>(sampleRate),
                                     toneData.getAmplitude(), samples);

                        totalSize += frameCount - offset;
                        neededSize -= frameCount - offset;
                        offset = frameCount;
                    }
                    else
                    {
                        generateWave(toneData.getType(), neededSize, offset,
                                     toneData.getFrequency() / static_cast<float>(sampleRate),
                                     toneData.getAmplitude(), samples);

                        totalSize += neededSize;
                        offset += neededSize;
                        neededSize = 0;
                    }

                    if (!isRepeating()) break;
                }

                if ((frameCount - offset) == 0)
                {
                    if (!isRepeating()) playing = false; // TODO: fire event
                    reset();
                }

                std::fill(samples.begin() + totalSize, samples.end(), 0.0F);
            }
            else
            {
                generateWave(toneData.getType(), frames, offset,
                             toneData.getFrequency() / static_cast<float>(sampleRate),
                             toneData.getAmplitude(), samples);

                offset += frames;
            }
        }

        ToneSound::ToneSound(Audio& initAudio, float initFrequency,
                             Type initType, float initAmplitude, float initLength):
            Sound(initAudio, initAudio.initSourceData(std::unique_ptr<SourceData>(new ToneData(initFrequency, initType, initAmplitude, initLength)))),
            type(initType),
            frequency(initFrequency),
            amplitude(initAmplitude),
            length(initLength)
        {
        }
    } // namespace audio
} // namespace ouzel
