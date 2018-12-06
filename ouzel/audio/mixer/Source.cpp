// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Source.hpp"
#include "Bus.hpp"
#include "SourceData.hpp"

namespace ouzel
{
    namespace audio
    {
        Source::Source(SourceData& initSourceData):
            sourceData(initSourceData)
        {
        }

        Source::~Source()
        {
            if (output) output->removeInput(this);
        }

        void Source::sampleData(std::vector<float>& samples, uint16_t& channels,
                                uint32_t& sampleRate, Vector3& position)
        {
            getData(samples);

            if (sampleRate != sourceData.getSampleRate())
            {
                // TODO: resample
            }

            if (channels > sourceData.getChannels())
            {
                // TODO: upmix
            }
            else if (channels < sourceData.getChannels())
            {
                // TODO: downmix
            }
        }

        void Source::setOutput(Bus* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            if (output) output->addInput(this);
        }

        void Source::play(bool repeat)
        {
            playing = true;
            repeating = repeat;
        }

        void Source::stop(bool shouldReset)
        {
            playing = false;
            if (shouldReset) reset();
        }
    } // namespace audio
} // namespace ouzel
