// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Bus.hpp"
#include "Processor.hpp"

namespace ouzel
{
    namespace audio
    {
        Bus::Bus()
        {
        }

        Bus::~Bus()
        {
            for (Processor* processor : processors)
                processor->bus = nullptr;
        }

        void Bus::setOutput(Bus* newOutput)
        {
            output = newOutput;
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

        void Bus::getData(std::vector<float>& samples, uint16_t& channels,
                          uint32_t& sampleRate, Vector3& position)
        {

        }
    } // namespace audio
} // namespace ouzel
