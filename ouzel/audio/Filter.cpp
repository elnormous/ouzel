// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Filter.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class FilterProcessor final: public Node
        {
        public:
            FilterProcessor()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
            }
        };

        Filter::Filter(Audio& initAudio):
            audio(initAudio),
            nodeId(audio.initNode([]() { return std::unique_ptr<Node>(new FilterProcessor()); }))
        {
        }

        Filter::~Filter()
        {
            if (nodeId) audio.deleteNode(nodeId);
        }
    } // namespace audio
} // namespace ouzel
