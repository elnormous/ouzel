// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Effect.hpp"
#include "Audio.hpp"
#include "Submix.hpp"

namespace ouzel
{
    namespace audio
    {
        Effect::Effect(Audio& initAudio,
                       uintptr_t initProcessorId):
            Node(initAudio),
            audio(initAudio),
            processorId(initProcessorId)
        {
        }

        Effect::~Effect()
        {
            if (processorId) audio.deleteObject(processorId);
        }

        void Effect::setEnabled(bool newEnabled)
        {
            enabled = newEnabled;

            /*audio.updateProcessor(processorId, [newEnabled](mixer::Object* node) {
                mixer::Processor* processor = static_cast<mixer::Processor*>(node);
                processor->setEnabled(newEnabled);
            });*/
        }
    } // namespace audio
} // namespace ouzel
