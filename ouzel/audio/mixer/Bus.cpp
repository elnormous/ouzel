// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Bus.hpp"
#include "Processor.hpp"
#include "Source.hpp"
#include "SourceData.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        Bus::Bus()
        {
        }

        Bus::~Bus()
        {
            if (output) output->removeInput(this);

            for (Bus* inputBus : inputBuses)
                inputBus->output = nullptr;

            for (Source* source : inputSources)
                source->output = nullptr;

            for (Processor* processor : processors)
                processor->bus = nullptr;
        }

        void Bus::setOutput(Bus* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            if (output) output->addInput(this);
        }

        static void resample(uint16_t channels, uint32_t sourceFrames, const std::vector<float>& sourceSamples,
                             uint32_t frames, std::vector<float>& samples)
        {
            if (sourceFrames != frames)
            {
                float sourceIncrement = static_cast<float>(sourceFrames - 1) / static_cast<float>(frames - 1);
                float sourcePosition = 0.0F;

                samples.resize(frames * channels);

                for (uint32_t frame = 0; frame < frames - 1; ++frame)
                {
                    uint32_t sourceCurrentFrame = static_cast<uint32_t>(sourcePosition);
                    float fraction = sourcePosition - sourceCurrentFrame;

                    uint32_t sourceNextFrame = sourceCurrentFrame + 1;

                    for (uint32_t channel = 0; channel < channels; ++channel)
                    {
                        uint32_t sourceCurrentPosition = sourceCurrentFrame * channels + channel;
                        uint32_t sourceNextPosition = sourceNextFrame * channels + channel;

                        samples[frame * channels + channel] = ouzel::lerp(sourceSamples[sourceCurrentPosition],
                                                                          sourceSamples[sourceNextPosition],
                                                                          fraction);
                    }

                    sourcePosition += sourceIncrement;
                }

                // fill the last frame of the destination with the last frame of the source
                for (uint32_t channel = 0; channel < channels; ++channel)
                    samples[(frames - 1) * channels + channel] = sourceSamples[(sourceFrames - 1) * channels + channel];
            }
            else
                samples = sourceSamples;
        }

        static void mix(uint32_t frames, uint16_t sourceChannels, const std::vector<float>& sourceSamples,
                        uint16_t channels, std::vector<float>& samples)
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
                                    samples[frame * channels + 0] = sourceSamples[frame]; // L = M
                                    samples[frame * channels + 1] = sourceSamples[frame]; // R = M
                                }
                                break;
                            case 4: // upmix 1 to 4
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = sourceSamples[frame]; // L = M
                                    samples[frame * channels + 1] = sourceSamples[frame]; // R = M
                                    samples[frame * channels + 2] = 0.0F; // SL = 0
                                    samples[frame * channels + 3] = 0.0F; // SR = 0
                                }
                                break;
                            case 6: // upmix 1 to 6
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = 0.0F; // L = 0
                                    samples[frame * channels + 1] = 0.0F; // R = 0
                                    samples[frame * channels + 2] = sourceSamples[frame]; // C = M
                                    samples[frame * channels + 3] = 0.0F; // LFE = 0
                                    samples[frame * channels + 4] = 0.0F; // SL = 0
                                    samples[frame * channels + 5] = 0.0F; // SR = 0
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
                                    samples[frame] = (sourceSamples[frame * sourceChannels + 0] +
                                                      sourceSamples[frame * sourceChannels + 1]) * 0.5F; // M = (L + R) * 0.5
                                break;
                            case 4: // upmix 2 to 4
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = sourceSamples[frame * sourceChannels + 0]; // L = L
                                    samples[frame * channels + 1] = sourceSamples[frame * sourceChannels + 1]; // R = R
                                    samples[frame * channels + 2] = 0.0F; // SL = 0
                                    samples[frame * channels + 3] = 0.0F; // SR = 0
                                }
                                break;
                            case 6: // upmix 2 to 6
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = sourceSamples[frame * sourceChannels + 0]; // L = L
                                    samples[frame * channels + 1] = sourceSamples[frame * sourceChannels + 1]; // R = R
                                    samples[frame * channels + 2] = 0.0F; // C = 0
                                    samples[frame * channels + 3] = 0.0F; // LFE = 0
                                    samples[frame * channels + 4] = 0.0F; // SL = 0
                                    samples[frame * channels + 5] = 0.0F; // SR = 0
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
                                    samples[frame] = (sourceSamples[frame * sourceChannels + 0] +
                                                      sourceSamples[frame * sourceChannels + 1] +
                                                      sourceSamples[frame * sourceChannels + 2] +
                                                      sourceSamples[frame * sourceChannels + 3]) * 0.25F; // M = (L + R + SL + SR) * 0.25
                                break;
                            case 2: // downmix 4 to 2
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = (sourceSamples[frame * sourceChannels + 0] +
                                                                     sourceSamples[frame * sourceChannels + 2]) * 0.5F; // L = (L + SL) * 0.5
                                    samples[frame * channels + 1] = (sourceSamples[frame * sourceChannels + 1] +
                                                                     sourceSamples[frame * sourceChannels + 3]) * 0.5F; // R = (R + RL) * 0.5
                                }
                                break;
                            case 6: // upmix 4 to 6
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = sourceSamples[frame * sourceChannels + 0]; // L = L
                                    samples[frame * channels + 1] = sourceSamples[frame * sourceChannels + 1]; // R = R
                                    samples[frame * channels + 2] = 0.0F; // C = 0
                                    samples[frame * channels + 3] = 0.0F; // LFE = 0
                                    samples[frame * channels + 4] = sourceSamples[frame * sourceChannels + 2]; // SL = SL
                                    samples[frame * channels + 5] = sourceSamples[frame * sourceChannels + 3]; // SR = SR
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
                                    samples[frame] = ((sourceSamples[frame * sourceChannels + 0] +
                                                       sourceSamples[frame * sourceChannels + 1]) * 0.7071F +
                                                      sourceSamples[frame * sourceChannels + 2] +
                                                      (sourceSamples[frame * sourceChannels + 4] +
                                                       sourceSamples[frame * sourceChannels + 5]) * 0.5F); // M = (L + R) * 0.7071 + C + (SL + SR) * 0.5
                                break;
                            case 2: // downmix 6 to 2
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = (sourceSamples[frame * sourceChannels + 0] +
                                                                     (sourceSamples[frame * sourceChannels + 2] +
                                                                      sourceSamples[frame * sourceChannels + 4]) * 0.7071F); // L = L + (C + SL) * 0.7071
                                    samples[frame * channels + 1] = (sourceSamples[frame * sourceChannels + 1] +
                                                                     (sourceSamples[frame * sourceChannels + 2] +
                                                                      sourceSamples[frame * sourceChannels + 5]) * 0.7071F); // R = R + (C + SR) * 0.7071
                                }
                                break;
                            case 4: // downmix 6 to 4
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = (sourceSamples[frame * sourceChannels + 0] +
                                                                     sourceSamples[frame * sourceChannels + 2] * 0.7071F); // L = L + C * 0.7071
                                    samples[frame * channels + 1] = (sourceSamples[frame * sourceChannels + 1] +
                                                                     sourceSamples[frame * sourceChannels + 2] * 0.7071F); // R = R + C * 0.7071
                                    samples[frame * channels + 2] = sourceSamples[frame * sourceChannels + 4]; // SL = SL
                                    samples[frame * channels + 3] = sourceSamples[frame * sourceChannels + 5]; // SR = SR
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

        void Bus::getData(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                          const Vector3& listenerPosition, const Quaternion& listenerRotation,
                          std::vector<float>& samples)
        {
            samples.resize(frames * channels);
            std::fill(samples.begin(), samples.end(), 0.0F);

            for (Bus* bus : inputBuses)
            {
                bus->getData(frames, channels, sampleRate, listenerPosition, listenerRotation, buffer);

                for (Processor* processor : processors)
                    processor->process(frames, channels, sampleRate, buffer);

                for (size_t s = 0; s < samples.size(); ++s)
                    samples[s] += buffer[s];
            }

            for (Source* source : inputSources)
            {
                if (source->isPlaying())
                {
                    const uint32_t sourceSampleRate = source->getSourceData().getSampleRate();
                    const uint16_t sourceChannels = source->getSourceData().getChannels();

                    if (sourceSampleRate != sampleRate)
                    {
                        uint32_t sourceFrames = (frames * sourceSampleRate + sampleRate - 1) / sampleRate; // round up
                        source->getData(sourceFrames, resampleBuffer);
                        resample(sourceChannels, sourceFrames, resampleBuffer, frames, mixBuffer);
                    }
                    else
                        source->getData(frames, mixBuffer);

                    if (sourceChannels != channels)
                        mix(frames, sourceChannels, mixBuffer, channels, buffer);
                    else
                        buffer = mixBuffer;

                    for (Processor* processor : processors)
                        processor->process(frames, channels, sampleRate, buffer);

                    for (size_t s = 0; s < samples.size(); ++s)
                        samples[s] += buffer[s];
                }
            }
        }

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

        void Bus::addProcessor(Processor* processor)
        {
            auto i = std::find(processors.begin(), processors.end(), processor);

            if (i == processors.end())
            {
                if (processor->bus) processor->bus->removeProcessor(processor);
                processor->bus = this;
                processors.push_back(processor);
            }
        }

        void Bus::removeProcessor(Processor* processor)
        {
            auto i = std::find(processors.begin(), processors.end(), processor);

            if (i != processors.end())
            {
                processor->bus = nullptr;
                processors.erase(i);
            }
        }
    } // namespace audio
} // namespace ouzel
