// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "OscillatorSound.hpp"
#include "Audio.hpp"
#include "mixer/Stream.hpp"
#include "mixer/Source.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        class OscillatorData;

        class OscillatorSource final: public mixer::Stream
        {
        public:
            OscillatorSource(OscillatorData& oscillatorData);

            void reset() override
            {
                position = 0;
            }

            void getData(uint32_t frames, std::vector<float>& samples) override;

        private:
            uint32_t position = 0;
        };

        class OscillatorData final: public mixer::Source
        {
        public:
            OscillatorData(float initFrequency, OscillatorSound::Type initType, float initAmplitude, float initLength):
                frequency(initFrequency),
                type(initType),
                amplitude(initAmplitude),
                length(initLength)
            {
                channels = 1;
                sampleRate = 44100;
            }

            float getFrequency() const { return frequency; }
            OscillatorSound::Type getType() const { return type; }
            float getAmplitude() const { return amplitude; }
            float getLength() const { return length; }

            std::unique_ptr<mixer::Stream> createStream() override
            {
                return std::unique_ptr<mixer::Stream>(new OscillatorSource(*this));
            }

        private:
            float frequency;
            OscillatorSound::Type type;
            float amplitude;
            float length;
        };

        OscillatorSource::OscillatorSource(OscillatorData& oscillatorData):
            Stream(oscillatorData)
        {
        }

        static void generateWave(OscillatorSound::Type type, uint32_t frames, uint32_t offset,
                                 float frameLength, float amplitude, float* samples)
        {
            for (uint32_t i = 0; i < frames; ++i)
            {
                float t = static_cast<float>(offset) * frameLength;

                switch (type)
                {
                    case OscillatorSound::Type::SINE:
                        samples[i] = sinf(t * tau<float>());
                        break;
                    case OscillatorSound::Type::SQUARE:
                        samples[i] = fmodf(roundf(t * 2.0F + 0.5F), 2.0F) * 2.0F - 1.0F;
                        break;
                    case OscillatorSound::Type::SAWTOOTH:
                        samples[i] = fmodf(t + 0.5F, 1.0F) * 2.0F - 1.0F;
                        break;
                    case OscillatorSound::Type::TRIANGLE:
                        samples[i] = fabsf(fmodf(t + 0.75F, 1.0F) * 2.0F - 1.0F) * 2.0F - 1.0F;
                        break;
                }

                samples[i] *= amplitude;

                ++offset;
            }
        }

        void OscillatorSource::getData(uint32_t frames, std::vector<float>& samples)
        {
            OscillatorData& oscillatorData = static_cast<OscillatorData&>(source);

            samples.resize(frames);

            const uint32_t sampleRate = source.getSampleRate();
            const float length = static_cast<OscillatorData&>(source).getLength();

            if (length > 0.0F)
            {
                const uint32_t frameCount = static_cast<uint32_t>(length * sampleRate);
                uint32_t neededSize = frames;
                uint32_t totalSize = 0;

                while (neededSize > 0)
                {
                    if (isRepeating() && (frameCount - position) == 0) reset();

                    if (frameCount - position < neededSize)
                    {
                        generateWave(oscillatorData.getType(), frameCount - position, position,
                                     oscillatorData.getFrequency() / static_cast<float>(sampleRate),
                                     oscillatorData.getAmplitude(), samples.data() + totalSize);

                        totalSize += frameCount - position;
                        neededSize -= frameCount - position;
                        position = frameCount;
                    }
                    else
                    {
                        generateWave(oscillatorData.getType(), neededSize, position,
                                     oscillatorData.getFrequency() / static_cast<float>(sampleRate),
                                     oscillatorData.getAmplitude(), samples.data() + totalSize);

                        totalSize += neededSize;
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

                std::fill(samples.begin() + totalSize, samples.end(), 0.0F);
            }
            else
            {
                generateWave(oscillatorData.getType(), frames, position,
                             oscillatorData.getFrequency() / static_cast<float>(sampleRate),
                             oscillatorData.getAmplitude(), samples.data());

                position += frames;
            }
        }

        OscillatorSound::OscillatorSound(Audio& initAudio, float initFrequency,
                                         Type initType, float initAmplitude, float initLength):
            Sound(initAudio, initAudio.initSource([initFrequency, initType, initAmplitude, initLength](){
                return std::unique_ptr<mixer::Source>(new OscillatorData(initFrequency, initType, initAmplitude, initLength));
            })),
            type(initType),
            frequency(initFrequency),
            amplitude(initAmplitude),
            length(initLength)
        {
        }
    } // namespace audio
} // namespace ouzel
