// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Listener.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "SoundInput.hpp"
#include "scene/Actor.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        class ListenerProcessor final: public Node
        {
        public:
            ListenerProcessor()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
                Node::process(samples, channels, sampleRate, position);
            }
        };

        Listener::Listener(Audio& initAudio):
            scene::Component(scene::Component::LISTENER),
            audio(initAudio),
            nodeId(audio.initNode([]() { return std::unique_ptr<Node>(new ListenerProcessor()); }))
        {
        }

        Listener::~Listener()
        {
            if (nodeId) audio.deleteNode(nodeId);
        }

        void Listener::updateTransform()
        {
            transformDirty = true;
        }
    } // namespace audio
} // namespace ouzel
