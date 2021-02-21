// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "Effect.hpp"
#include "Audio.hpp"
#include "Submix.hpp"

namespace ouzel::audio
{
    Effect::Effect(Audio& initAudio,
                   std::size_t initProcessorId):
        Node(initAudio),
        audio(initAudio),
        processorId(initProcessorId)
    {
    }

    Effect::~Effect()
    {
        if (mix) mix->removeEffect(this);
        if (processorId) audio.deleteObject(processorId);
    }

    void Effect::setEnabled(bool newEnabled)
    {
        enabled = newEnabled;

        audio.updateProcessor(processorId, [newEnabled](mixer::Object* node) {
            auto processor = static_cast<mixer::Processor*>(node);
            processor->setEnabled(newEnabled);
        });
    }
}
