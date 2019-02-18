// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Effect.hpp"
#include "Audio.hpp"
#include "Submix.hpp"

namespace ouzel
{
    namespace audio
    {
        Effect::Effect(Audio& initAudio,
                       uintptr_t initProcessorId):
            audio(initAudio),
            processorId(initProcessorId)
        {
        }

        Effect::~Effect()
        {
            if (mix) mix->removeEffect(this);
            if (processorId) audio.deleteObject(processorId);
        }
    } // namespace audio
} // namespace ouzel
