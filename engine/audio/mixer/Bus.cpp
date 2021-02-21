// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Bus.hpp"
#include "Data.hpp"
#include "Processor.hpp"
#include "Stream.hpp"
#include "../../math/MathUtils.hpp"

namespace ouzel::audio::mixer
{
    Bus::~Bus()
    {
        if (output) output->removeInput(this);

        for (Bus* inputBus : inputBuses)
            inputBus->output = nullptr;

        for (Stream* stream : inputStreams)
            stream->output = nullptr;

        for (Processor* processor : processors)
            processor->bus = nullptr;
    }

    void Bus::setOutput(Bus* newOutput)
    {
        if (output) output->removeInput(this);
        output = newOutput;
        if (output) output->addInput(this);
    }

    static void resample(std::uint32_t channels, std::uint32_t sourceFrames, const std::vector<float>& sourceSamples,
                         std::uint32_t frames, std::vector<float>& samples)
    {
        if (sourceFrames != frames)
        {
            auto sourceIncrement = static_cast<float>(sourceFrames - 1) / static_cast<float>(frames - 1);
            auto sourcePosition = 0.0F;

            samples.resize(frames * channels);

            for (std::uint32_t frame = 0; frame < frames - 1; ++frame)
            {
                auto sourceCurrentFrame = static_cast<std::uint32_t>(sourcePosition);
                auto fraction = sourcePosition - sourceCurrentFrame;

                std::uint32_t sourceNextFrame = sourceCurrentFrame + 1;

                for (std::uint32_t channel = 0; channel < channels; ++channel)
                {
                    const auto sourceChannel = &sourceSamples[channel * sourceFrames];
                    auto outputChannel = &samples[channel * frames];

                    outputChannel[frame] = lerp(sourceChannel[sourceCurrentFrame],
                                                sourceChannel[sourceNextFrame],
                                                fraction);
                }

                sourcePosition += sourceIncrement;
            }

            // fill the last frame of the destination with the last frame of the source
            for (std::uint32_t channel = 0; channel < channels; ++channel)
            {
                const auto sourceChannel = &sourceSamples[channel * sourceFrames];
                auto outputChannel = &samples[channel * frames];
                outputChannel[frames - 1] = sourceChannel[sourceFrames - 1];
            }
        }
        else
            samples = sourceSamples;
    }

    static void convert(std::uint32_t frames, std::uint32_t sourceChannels, const std::vector<float>& sourceSamples,
                        std::uint32_t channels, std::vector<float>& samples)
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
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                            {
                                samples[0 * frames + frame] = sourceSamples[frame]; // L = M
                                samples[1 * frames + frame] = sourceSamples[frame]; // R = M
                            }
                            break;
                        case 4: // upmix 1 to 4
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                            {
                                samples[0 * frames + frame] = sourceSamples[frame]; // L = M
                                samples[1 * frames + frame] = sourceSamples[frame]; // R = M
                                samples[2 * frames + frame] = 0.0F; // SL = 0
                                samples[3 * frames + frame] = 0.0F; // SR = 0
                            }
                            break;
                        case 6: // upmix 1 to 6
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
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
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                                samples[frame] = (sourceSamples[0 * frames + frame] +
                                                  sourceSamples[1 * frames + frame]) * 0.5F; // M = (L + R) * 0.5
                            break;
                        case 4: // upmix 2 to 4
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                            {
                                samples[0 * frames + frame] = sourceSamples[0 * frames + frame]; // L = L
                                samples[1 * frames + frame] = sourceSamples[1 * frames + frame]; // R = R
                                samples[2 * frames + frame] = 0.0F; // SL = 0
                                samples[3 * frames + frame] = 0.0F; // SR = 0
                            }
                            break;
                        case 6: // upmix 2 to 6
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
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
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                                samples[frame] = (sourceSamples[0 * frames + frame] +
                                                  sourceSamples[1 * frames + frame] +
                                                  sourceSamples[2 * frames + frame] +
                                                  sourceSamples[3 * frames + frame]) * 0.25F; // M = (L + R + SL + SR) * 0.25
                            break;
                        case 2: // downmix 4 to 2
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                            {
                                samples[0 * frames + frame] = (sourceSamples[0 * frames + frame] +
                                                               sourceSamples[2 * frames + frame]) * 0.5F; // L = (L + SL) * 0.5
                                samples[1 * frames + frame] = (sourceSamples[1 * frames + frame] +
                                                               sourceSamples[3 * frames + frame]) * 0.5F; // R = (R + RL) * 0.5
                            }
                            break;
                        case 6: // upmix 4 to 6
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
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
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
                                samples[frame] = ((sourceSamples[0 * frames + frame] +
                                                   sourceSamples[1 * frames + frame]) * 0.7071F +
                                                  sourceSamples[2 * frames + frame] +
                                                  (sourceSamples[4 * frames + frame] +
                                                   sourceSamples[5 * frames + frame]) * 0.5F); // M = (L + R) * 0.7071 + C + (SL + SR) * 0.5
                            break;
                        case 2: // downmix 6 to 2
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
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
                            for (std::uint32_t frame = 0; frame < frames; ++frame)
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

    void Bus::generateSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate,
                              const Vector3F& listenerPosition, const QuaternionF& listenerRotation,
                              std::vector<float>& samples)
    {
        samples.resize(frames * channels);
        std::fill(samples.begin(), samples.end(), 0.0F);

        for (Bus* bus : inputBuses)
        {
            bus->generateSamples(frames, channels, sampleRate,
                                 listenerPosition, listenerRotation, buffer);

            for (std::size_t s = 0; s < samples.size(); ++s)
                samples[s] += buffer[s];
        }

        for (Stream* stream : inputStreams)
        {
            if (stream->isPlaying())
            {
                const std::uint32_t sourceSampleRate = stream->getData().getSampleRate();
                const std::uint32_t sourceChannels = stream->getData().getChannels();

                if (sourceSampleRate != sampleRate)
                {
                    std::uint32_t sourceFrames = (frames * sourceSampleRate + sampleRate - 1) / sampleRate; // round up
                    stream->generateSamples(sourceFrames, resampleBuffer);
                    resample(sourceChannels, sourceFrames, resampleBuffer, frames, mixBuffer);
                }
                else
                    stream->generateSamples(frames, mixBuffer);

                if (sourceChannels != channels)
                    convert(frames, sourceChannels, mixBuffer, channels, buffer);
                else
                    buffer = mixBuffer;

                for (std::size_t s = 0; s < samples.size(); ++s)
                    samples[s] += buffer[s];
            }
        }

        for (Processor* processor : processors)
            if (processor->isEnabled())
                processor->process(frames, channels, sampleRate, samples);
    }

    void Bus::addProcessor(Processor* processor)
    {
        const auto i = std::find(processors.begin(), processors.end(), processor);
        if (i == processors.end())
        {
            if (processor->bus) processor->bus->removeProcessor(processor);
            processor->bus = this;
            processors.push_back(processor);
        }
    }

    void Bus::removeProcessor(Processor* processor)
    {
        const auto i = std::find(processors.begin(), processors.end(), processor);
        if (i != processors.end())
        {
            processor->bus = nullptr;
            processors.erase(i);
        }
    }

    void Bus::addInput(Bus* bus)
    {
        const auto i = std::find(inputBuses.begin(), inputBuses.end(), bus);
        if (i == inputBuses.end()) inputBuses.push_back(bus);
    }

    void Bus::removeInput(Bus* bus)
    {
        const auto i = std::find(inputBuses.begin(), inputBuses.end(), bus);
        if (i != inputBuses.end()) inputBuses.erase(i);
    }

    void Bus::addInput(Stream* stream)
    {
        const auto i = std::find(inputStreams.begin(), inputStreams.end(), stream);
        if (i == inputStreams.end()) inputStreams.push_back(stream);
    }

    void Bus::removeInput(Stream* stream)
    {
        const auto i = std::find(inputStreams.begin(), inputStreams.end(), stream);
        if (i != inputStreams.end()) inputStreams.erase(i);
    }
}
