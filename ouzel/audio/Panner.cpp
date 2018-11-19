// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Panner.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class PannerProcessor final: public Node
        {
        public:
            PannerProcessor()
            {
            }

            void process(std::vector<float>& samples, uint32_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
            }
        };

        Panner::Panner(Audio& initAudio):
            audio(initAudio),
            nodeId(audio.initNode([]() { return std::unique_ptr<Node>(new PannerProcessor()); }))
        {
        }

        Panner::~Panner()
        {
            if (nodeId) audio.deleteNode(nodeId);
        }
    } // namespace audio
} // namespace ouzel
