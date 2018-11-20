// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Pitch.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class PitchProcessor final: public Node
        {
        public:
            PitchProcessor()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
            }
        };

        Pitch::Pitch(Audio& initAudio):
            audio(initAudio),
            nodeId(audio.initNode([]() { return std::unique_ptr<Node>(new PitchProcessor()); }))
        {
        }

        Pitch::~Pitch()
        {
            if (nodeId) audio.deleteNode(nodeId);
        }

        void Pitch::setPitch(float newPitch)
        {
            pitch = newPitch;
        }
    } // namespace audio
} // namespace ouzel
