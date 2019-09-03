// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Bus.hpp"
#include "Data.hpp"
#include "Processor.hpp"
#include "Source.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            void Bus::addInput(Bus* bus)
            {
                auto i = std::find(inputBuses.begin(), inputBuses.end(), bus);
                if (i == inputBuses.end()) inputBuses.push_back(bus);
            }

            void Bus::removeInput(Bus* bus)
            {
                auto i = std::find(inputBuses.begin(), inputBuses.end(), bus);
                if (i != inputBuses.end()) inputBuses.erase(i);
            }

            void Bus::addInput(Source* source)
            {
                auto i = std::find(inputSources.begin(), inputSources.end(), source);
                if (i == inputSources.end()) inputSources.push_back(source);
            }

            void Bus::removeInput(Source* source)
            {
                auto i = std::find(inputSources.begin(), inputSources.end(), source);
                if (i != inputSources.end()) inputSources.erase(i);
            }

            static void resample(uint32_t channels, uint32_t sourceFrames, const std::vector<float>& sourceSamples,
                                 uint32_t frames, std::vector<float>& samples)
            {
                if (sourceFrames != frames)
                {
                    auto sourceIncrement = static_cast<float>(sourceFrames - 1) / static_cast<float>(frames - 1);
                    auto sourcePosition = 0.0F;

                    samples.resize(frames * channels);

                    for (uint32_t frame = 0; frame < frames - 1; ++frame)
                    {
                        auto sourceCurrentFrame = static_cast<uint32_t>(sourcePosition);
                        auto fraction = sourcePosition - sourceCurrentFrame;

                        uint32_t sourceNextFrame = sourceCurrentFrame + 1;

                        for (uint32_t channel = 0; channel < channels; ++channel)
                        {
                            const auto sourceChannel = &sourceSamples[channel * sourceFrames];
                            auto outputChannel = &samples[channel * frames];

                            outputChannel[frame] = ouzel::lerp(sourceChannel[sourceCurrentFrame],
                                                               sourceChannel[sourceNextFrame],
                                                               fraction);
                        }

                        sourcePosition += sourceIncrement;
                    }

                    // fill the last frame of the destination with the last frame of the source
                    for (uint32_t channel = 0; channel < channels; ++channel)
                    {
                        const auto sourceChannel = &sourceSamples[channel * sourceFrames];
                        auto outputChannel = &samples[channel * frames];
                        outputChannel[frames - 1] = sourceChannel[sourceFrames - 1];
                    }
                }
                else
                    samples = sourceSamples;
            }

            static void convert(uint32_t frames, uint32_t sourceChannels, const std::vector<float>& sourceSamples,
                                uint32_t channels, std::vector<float>& samples)
            {
                samples.resize(frames * channels);

                if (sourceChannels != channels)
                {
                    switch (sourceChannels)
                    {
                        case 1:
                        {
                            switch (channels)
                            {
                                case 2: // upmix 1 to 2
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                    {
                                        samples[0 * frames + frame] = sourceSamples[frame]; // L = M
                                        samples[1 * frames + frame] = sourceSamples[frame]; // R = M
                                    }
                                    break;
                                case 4: // upmix 1 to 4
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                    {
                                        samples[0 * frames + frame] = sourceSamples[frame]; // L = M
                                        samples[1 * frames + frame] = sourceSamples[frame]; // R = M
                                        samples[2 * frames + frame] = 0.0F; // SL = 0
                                        samples[3 * frames + frame] = 0.0F; // SR = 0
                                    }
                                    break;
                                case 6: // upmix 1 to 6
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                    {
                                        samples[0 * frames + frame] = 0.0F; // L = 0
                                        samples[1 * frames + frame] = 0.0F; // R = 0
                                        samples[2 * frames + frame] = sourceSamples[frame]; // C = M
                                        samples[3 * frames + frame] = 0.0F; // LFE = 0
                                        samples[4 * frames + frame] = 0.0F; // SL = 0
                                        samples[5 * frames + frame] = 0.0F; // SR = 0
                                    }
                                    break;
                            }
                            break;
                        }
                        case 2:
                        {
                            switch (channels)
                            {
                                case 1: // downmix 2 to 1
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                        samples[frame] = (sourceSamples[0 * frames + frame] +
                                                          sourceSamples[1 * frames + frame]) * 0.5F; // M = (L + R) * 0.5
                                    break;
                                case 4: // upmix 2 to 4
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                    {
                                        samples[0 * frames + frame] = sourceSamples[0 * frames + frame]; // L = L
                                        samples[1 * frames + frame] = sourceSamples[1 * frames + frame]; // R = R
                                        samples[2 * frames + frame] = 0.0F; // SL = 0
                                        samples[3 * frames + frame] = 0.0F; // SR = 0
                                    }
                                    break;
                                case 6: // upmix 2 to 6
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                    {
                                        samples[0 * frames + frame] = sourceSamples[0 * frames + frame]; // L = L
                                        samples[1 * frames + frame] = sourceSamples[1 * frames + frame]; // R = R
                                        samples[2 * frames + frame] = 0.0F; // C = 0
                                        samples[3 * frames + frame] = 0.0F; // LFE = 0
                                        samples[4 * frames + frame] = 0.0F; // SL = 0
                                        samples[5 * frames + frame] = 0.0F; // SR = 0
                                    }
                                    break;
                            }
                            break;
                        }
                        case 4:
                        {
                            switch (channels)
                            {
                                case 1: // downmix 4 to 1
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                        samples[frame] = (sourceSamples[0 * frames + frame] +
                                                          sourceSamples[1 * frames + frame] +
                                                          sourceSamples[2 * frames + frame] +
                                                          sourceSamples[3 * frames + frame]) * 0.25F; // M = (L + R + SL + SR) * 0.25
                                    break;
                                case 2: // downmix 4 to 2
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                    {
                                        samples[0 * frames + frame] = (sourceSamples[0 * frames + frame] +
                                                                       sourceSamples[2 * frames + frame]) * 0.5F; // L = (L + SL) * 0.5
                                        samples[1 * frames + frame] = (sourceSamples[1 * frames + frame] +
                                                                       sourceSamples[3 * frames + frame]) * 0.5F; // R = (R + RL) * 0.5
                                    }
                                    break;
                                case 6: // upmix 4 to 6
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                    {
                                        samples[0 * frames + frame] = sourceSamples[0 * frames + frame]; // L = L
                                        samples[1 * frames + frame] = sourceSamples[1 * frames + frame]; // R = R
                                        samples[2 * frames + frame] = 0.0F; // C = 0
                                        samples[3 * frames + frame] = 0.0F; // LFE = 0
                                        samples[4 * frames + frame] = sourceSamples[2 * frames + frame]; // SL = SL
                                        samples[5 * frames + frame] = sourceSamples[3 * frames + frame]; // SR = SR
                                    }
                                    break;
                            }
                            break;
                        }
                        case 6:
                        {
                            switch (channels)
                            {
                                case 1: // downmix 6 to 1
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                        samples[frame] = ((sourceSamples[0 * frames + frame] +
                                                           sourceSamples[1 * frames + frame]) * 0.7071F +
                                                          sourceSamples[2 * frames + frame] +
                                                          (sourceSamples[4 * frames + frame] +
                                                           sourceSamples[5 * frames + frame]) * 0.5F); // M = (L + R) * 0.7071 + C + (SL + SR) * 0.5
                                    break;
                                case 2: // downmix 6 to 2
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                    {
                                        samples[frame * channels + 0] = (sourceSamples[0 * frames + frame] +
                                                                         (sourceSamples[2 * frames + frame] +
                                                                          sourceSamples[4 * frames + frame]) * 0.7071F); // L = L + (C + SL) * 0.7071
                                        samples[frame * channels + 1] = (sourceSamples[1 * frames + frame] +
                                                                         (sourceSamples[2 * frames + frame] +
                                                                          sourceSamples[5 * frames + frame]) * 0.7071F); // R = R + (C + SR) * 0.7071
                                    }
                                    break;
                                case 4: // downmix 6 to 4
                                    for (uint32_t frame = 0; frame < frames; ++frame)
                                    {
                                        samples[frame * channels + 0] = (sourceSamples[0 * frames + frame] +
                                                                         sourceSamples[2 * frames + frame] * 0.7071F); // L = L + C * 0.7071
                                        samples[frame * channels + 1] = (sourceSamples[1 * frames + frame] +
                                                                         sourceSamples[2 * frames + frame] * 0.7071F); // R = R + C * 0.7071
                                        samples[frame * channels + 2] = sourceSamples[4 * frames + frame]; // SL = SL
                                        samples[frame * channels + 3] = sourceSamples[5 * frames + frame]; // SR = SR
                                    }
                                    break;
                            }
                            break;
                        }
                    }
                }
                else
                    samples = sourceSamples;
            }

            void Bus::getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                                 std::vector<float>& samples)
            {
                samples.resize(frames * channels);
                std::fill(samples.begin(), samples.end(), 0.0F);

                for (Bus* bus : inputBuses)
                {
                    bus->getSamples(frames, channels, sampleRate, buffer);

                    for (size_t s = 0; s < samples.size(); ++s)
                        samples[s] += buffer[s];
                }

                for (Source* source : inputSources)
                {
                    source->getSamples(frames, channels, sampleRate, buffer);

                    for (size_t s = 0; s < samples.size(); ++s)
                        samples[s] += buffer[s];

                    /*if (source->isPlaying())
                    {
                        const uint32_t sourceSampleRate = source->getData().getSampleRate();
                        const uint16_t sourceChannels = source->getData().getChannels();

                        if (sourceSampleRate != sampleRate)
                        {
                            uint32_t sourceFrames = (frames * sourceSampleRate + sampleRate - 1) / sampleRate; // round up
                            source->getSamples(sourceFrames, resampleBuffer);
                            resample(sourceChannels, sourceFrames, resampleBuffer, frames, mixBuffer);
                        }
                        else
                            source->getSamples(frames, mixBuffer);

                        if (sourceChannels != channels)
                            convert(frames, sourceChannels, mixBuffer, channels, buffer);
                        else
                            buffer = mixBuffer;

                        for (size_t s = 0; s < samples.size(); ++s)
                            samples[s] += buffer[s];
                    }*/
                }

                if (processor && processor->isEnabled())
                    processor->process(frames, channels, sampleRate, samples);
            }
        }
    } // namespace audio
} // namespace ouzel
