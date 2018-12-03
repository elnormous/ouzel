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
    } // namespace audio
} // namespace ouzel
