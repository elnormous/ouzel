// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Gain.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class GainProcessor final: public Node
        {
        public:
            GainProcessor()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
                Node::process(samples, channels, sampleRate, position);

                for (float& sample : samples)
                    sample *= gain;
            }

            void setGain(float newGain)
            {
                gain = newGain;
            }

        private:
            float gain = 1.0F;
        };

        Gain::Gain(Audio& initAudio):
            Filter(initAudio),
            nodeId(audio.initNode([]() { return std::unique_ptr<Node>(new GainProcessor()); }))
        {
        }

        Gain::~Gain()
        {
            if (nodeId) audio.deleteNode(nodeId);
        }

        void Gain::setGain(float newGain)
        {
            gain = newGain;

            audio.updateNode(nodeId, [newGain](Node* node) {
                GainProcessor* gainProcessor = static_cast<GainProcessor*>(node);
                gainProcessor->setGain(newGain);
            });
        }
    } // namespace audio
} // namespace ouzel
