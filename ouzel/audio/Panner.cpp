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

            void process(std::vector<float>&, uint16_t&,
                         uint32_t&, Vector3& pos) override
            {
                position = pos;
            }

            void setPosition(const Vector3& newPosition)
            {
                position = newPosition;
            }

        private:
            Vector3 position;
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

        void Panner::setPosition(const Vector3& newPosition)
        {
            position = newPosition;

            audio.updateNode(nodeId, [newPosition](Node* node) {
                PannerProcessor* pannerProcessor = static_cast<PannerProcessor*>(node);
                pannerProcessor->setPosition(newPosition);
            });
        }
    } // namespace audio
} // namespace ouzel
