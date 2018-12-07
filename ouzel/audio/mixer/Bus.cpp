// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Bus.hpp"
#include "Processor.hpp"
#include "Source.hpp"

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

        void Bus::getData(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                          Vector3 listenerPosition, std::vector<float>& samples)
        {
            samples.resize(frames * channels);
            std::fill(samples.begin(), samples.end(), 0.0F);

            for (Bus* bus : inputBuses)
            {
                bus->getData(frames, channels, sampleRate, listenerPosition, buffer);

                for (size_t s = 0; s < samples.size(); ++s)
                    samples[s] += buffer[s];

            }

            for (Source* source : inputSources)
            {
                if (source->isPlaying())
                {
                    source->sampleData(frames, channels, sampleRate, listenerPosition, buffer);

                    for (size_t s = 0; s < samples.size(); ++s)
                        samples[s] += buffer[s];
                }
            }

            for (Processor* processor : processors)
                processor->process(frames, channels, sampleRate, listenerPosition, samples);
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
