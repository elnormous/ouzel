// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Oscillator.hpp"
#include "Audio.hpp"
#include "mixer/Data.hpp"
#include "math/Constants.hpp"

namespace ouzel
{
    namespace audio
    {
        static void generateWave(Oscillator::Type type, uint32_t frames, uint32_t offset,
                                 float frameLength, float amplitude, float* samples)
        {
            for (uint32_t i = 0; i < frames; ++i)
            {
                auto t = static_cast<float>(offset) * frameLength;

                switch (type)
                {
                    case Oscillator::Type::Sine:
                        samples[i] = sin(t * tau<float>);
                        break;
                    case Oscillator::Type::Square:
                        samples[i] = fmod(round(t * 2.0F + 0.5F), 2.0F) * 2.0F - 1.0F;
                        break;
                    case Oscillator::Type::Sawtooth:
                        samples[i] = fmod(t + 0.5F, 1.0F) * 2.0F - 1.0F;
                        break;
                    case Oscillator::Type::Triangle:
                        samples[i] = fabs(fmod(t + 0.75F, 1.0F) * 2.0F - 1.0F) * 2.0F - 1.0F;
                        break;
                }

                samples[i] *= amplitude;

                ++offset;
            }
        }

        Oscillator::Oscillator(float initFrequency, Type initType,
                               float initAmplitude, float initLength):
            type(initType),
            frequency(initFrequency),
            amplitude(initAmplitude),
            length(initLength)
        {
        }

        void Oscillator::reset()
        {
            position = 0;
        }

        void Oscillator::getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)
        {
            if (length > 0.0F)
            {
                const auto frameCount = static_cast<uint32_t>(length * sampleRate);
                auto neededSize = frames;
                uint32_t totalSize = 0;

                if (neededSize > 0)
                {
                    if (frameCount - position < neededSize)
                    {
                        generateWave(type, frameCount - position, position,
                                     frequency / static_cast<float>(sampleRate),
                                     amplitude, samples.data() + totalSize);

                        totalSize += frameCount - position;
                        neededSize -= frameCount - position;
                        position = frameCount;
                    }
                    else
                    {
                        generateWave(type, neededSize, position,
                                     frequency / static_cast<float>(sampleRate),
                                     amplitude, samples.data() + totalSize);

                        totalSize += neededSize;
                        position += neededSize;
                        neededSize = 0;
                    }
                }

                if ((frameCount - position) == 0)
                    stop(true);

                std::fill(samples.begin() + totalSize, samples.end(), 0.0F); // TODO: remove
            }
            else
            {
                generateWave(type, frames, position,
                             frequency / static_cast<float>(sampleRate),
                             amplitude, samples.data());

                position += frames;
            }
        }
    } // namespace audio
} // namespace ouzel
