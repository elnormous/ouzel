// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Panner.hpp"
#include "Audio.hpp"
#include "scene/Actor.hpp"

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

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& pos) override
            {
                Node::process(samples, channels, sampleRate, pos);

                position = pos;
            }

            void setPosition(const Vector3& newPosition)
            {
                position = newPosition;
            }

            void setRolloffFactor(float newRolloffFactor)
            {
                rolloffFactor = newRolloffFactor;
            }

            void setMinDistance(float newMinDistance)
            {
                minDistance = newMinDistance;
            }

            void setMaxDistance(float newMaxDistance)
            {
                maxDistance = newMaxDistance;
            }

        private:
            Vector3 position;
            float rolloffFactor = 1.0F;
            float minDistance = 1.0F;
            float maxDistance = FLT_MAX;
        };

        Panner::Panner(Audio& initAudio):
            scene::Component(scene::Component::SOUND),
            Filter(initAudio),
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

        void Panner::setRolloffFactor(float newRolloffFactor)
        {
            rolloffFactor = newRolloffFactor;

            audio.updateNode(nodeId, [newRolloffFactor](Node* node) {
                PannerProcessor* pannerProcessor = static_cast<PannerProcessor*>(node);
                pannerProcessor->setRolloffFactor(newRolloffFactor);
            });
        }

        void Panner::setMinDistance(float newMinDistance)
        {
            minDistance = newMinDistance;

            audio.updateNode(nodeId, [newMinDistance](Node* node) {
                PannerProcessor* pannerProcessor = static_cast<PannerProcessor*>(node);
                pannerProcessor->setMinDistance(newMinDistance);
            });
        }

        void Panner::setMaxDistance(float newMaxDistance)
        {
            maxDistance = newMaxDistance;

            audio.updateNode(nodeId, [newMaxDistance](Node* node) {
                PannerProcessor* pannerProcessor = static_cast<PannerProcessor*>(node);
                pannerProcessor->setMaxDistance(newMaxDistance);
            });
        }

        void Panner::updateTransform()
        {
            setPosition(actor->getWorldPosition());
        }
    } // namespace audio
} // namespace ouzel
