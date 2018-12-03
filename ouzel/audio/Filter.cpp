// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Filter.hpp"
#include "Audio.hpp"
#include "Submix.hpp"

namespace ouzel
{
    namespace audio
    {
        Filter::Filter(Audio& initAudio,
                       uintptr_t initProcessorId):
            audio(initAudio),
            processorId(initProcessorId)
        {
        }

        Filter::~Filter()
        {
            if (mix) mix->removeFilter(this);
            if (processorId) audio.deleteObject(processorId);
        }

        void Filter::setMix(Mix* newMix)
        {
            if (mix) mix->removeFilter(this);
            mix = newMix;
            if (mix) mix->addFilter(this);

            audio.getMixer().addCommand(std::unique_ptr<Command>(new SetProcessorBusCommand(processorId,
                                                                                            mix ? mix->getBusId() : 0)));
        }
    } // namespace audio
} // namespace ouzel
