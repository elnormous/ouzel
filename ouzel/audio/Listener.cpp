// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Listener.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "scene/Actor.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        class ListenerProcessor final: public Processor
        {
        public:
            ListenerProcessor()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
                Object::process(samples, channels, sampleRate, position);
            }
        };

        Listener::Listener(Audio& initAudio):
            audio(initAudio),
            scene::Component(scene::Component::LISTENER),
            objectId(audio.initObject([]() { return std::unique_ptr<Processor>(new ListenerProcessor()); }))
        {
        }

        Listener::~Listener()
        {
            if (objectId) audio.deleteObject(objectId);
        }

        void Listener::updateTransform()
        {
            transformDirty = true;
        }
    } // namespace audio
} // namespace ouzel
