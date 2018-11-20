// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Delay.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class DelayProcessor final: public Node
        {
        public:
            DelayProcessor()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
                Node::process(samples, channels, sampleRate, position);
            }
        };

        Delay::Delay(Audio& initAudio):
            audio(initAudio),
            nodeId(audio.initNode([]() { return std::unique_ptr<Node>(new DelayProcessor()); }))
        {
        }

        Delay::~Delay()
        {
            if (nodeId) audio.deleteNode(nodeId);
        }

        void Delay::setDelay(float newDelay)
        {
            delay = newDelay;
        }
    } // namespace audio
} // namespace ouzel
