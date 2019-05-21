// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Oscillator.hpp"
#include "Audio.hpp"
#include "mixer/Data.hpp"
#include "mixer/Stream.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        class OscillatorData;

        class OscillatorStream final: public mixer::Stream
        {
        public:
            OscillatorStream(OscillatorData& oscillatorData);

            void reset() override
            {
                position = 0;
            }

            void getSamples(uint32_t frames, std::vector<float>& samples) override;

        private:
            uint32_t position = 0;
        };

        class OscillatorData final: public mixer::Data
        {
        public:
            OscillatorData(float initFrequency, Oscillator::Type initType, float initAmplitude, float initLength):
                frequency(initFrequency),
                type(initType),
                amplitude(initAmplitude),
                length(initLength)
            {
                channels = 1;
                sampleRate = 44100;
            }

            inline float getFrequency() const { return frequency; }
            inline Oscillator::Type getType() const { return type; }
            inline float getAmplitude() const { return amplitude; }
            inline float getLength() const { return length; }

            std::unique_ptr<mixer::Stream> createStream() override
            {
                return std::unique_ptr<mixer::Stream>(new OscillatorStream(*this));
            }

        private:
            float frequency;
            Oscillator::Type type;
            float amplitude;
            float length;
        };

        OscillatorStream::OscillatorStream(OscillatorData& oscillatorData):
            Stream(oscillatorData)
        {
        }

        static void generateWave(Oscillator::Type type, uint32_t frames, uint32_t offset,
                                 float frameLength, float amplitude, float* samples)
        {
            for (uint32_t i = 0; i < frames; ++i)
            {
                float t = static_cast<float>(offset) * frameLength;

                switch (type)
                {
                    case Oscillator::Type::SINE:
                        samples[i] = sin(t * tau<float>());
                        break;
                    case Oscillator::Type::SQUARE:
                        samples[i] = fmod(round(t * 2.0F + 0.5F), 2.0F) * 2.0F - 1.0F;
                        break;
                    case Oscillator::Type::SAWTOOTH:
                        samples[i] = fmod(t + 0.5F, 1.0F) * 2.0F - 1.0F;
                        break;
                    case Oscillator::Type::TRIANGLE:
                        samples[i] = fabs(fmod(t + 0.75F, 1.0F) * 2.0F - 1.0F) * 2.0F - 1.0F;
                        break;
                }

                samples[i] *= amplitude;

                ++offset;
            }
        }

        void OscillatorStream::getSamples(uint32_t frames, std::vector<float>& samples)
        {
            OscillatorData& oscillatorData = static_cast<OscillatorData&>(data);

            samples.resize(frames);

            const uint32_t sampleRate = data.getSampleRate();
            const float length = static_cast<OscillatorData&>(data).getLength();

            if (length > 0.0F)
            {
                const uint32_t frameCount = static_cast<uint32_t>(length * sampleRate);
                uint32_t neededSize = frames;
                uint32_t totalSize = 0;

                if (neededSize > 0)
                {
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
                }

                if ((frameCount - position) == 0)
                {
                    playing = false; // TODO: fire event
                    reset();
                }

                std::fill(samples.begin() + totalSize, samples.end(), 0.0F); // TODO: remove
            }
            else
            {
                generateWave(oscillatorData.getType(), frames, position,
                             oscillatorData.getFrequency() / static_cast<float>(sampleRate),
                             oscillatorData.getAmplitude(), samples.data());

                position += frames;
            }
        }

        Oscillator::Oscillator(Audio& initAudio, float initFrequency,
                               Type initType, float initAmplitude, float initLength):
            Sound(initAudio,
                  initAudio.initData(std::unique_ptr<mixer::Data>(data = new OscillatorData(initFrequency, initType, initAmplitude, initLength))),
                  Sound::Format::PCM),
            type(initType),
            frequency(initFrequency),
            amplitude(initAmplitude),
            length(initLength)
        {
        }
    } // namespace audio
} // namespace ouzel
