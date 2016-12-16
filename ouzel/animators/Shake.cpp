// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <limits>
#include "Shake.h"
#include "scene/Node.h"
#include "math/MathUtils.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace scene
    {
        Shake::Shake(float aLength, const Vector3& aDistance, float aTimeScale):
            Animator(aLength), distance(aDistance), timeScale(aTimeScale)
        {
            seedX = std::uniform_int_distribution<uint32_t>{0, std::numeric_limits<uint32_t>::max()}(randomEngine);
            seedY = std::uniform_int_distribution<uint32_t>{0, std::numeric_limits<uint32_t>::max()}(randomEngine);
            seedZ = std::uniform_int_distribution<uint32_t>{0, std::numeric_limits<uint32_t>::max()}(randomEngine);
        }

        void Shake::start(Node* newTargetNode)
        {
            Animator::start(newTargetNode);

            if (targetNode)
            {
                startPosition = targetNode->getPosition();
            }
        }

        void Shake::updateProgress()
        {
            Animator::updateProgress();

            if (targetNode)
            {
                float x = length * progress * timeScale;

                uint64_t x1 = static_cast<uint32_t>(x);
                uint64_t x2 = x1 + 1;
                float t = x - static_cast<float>(x1);

                Vector3 previousPosition;
                Vector3 nextPosition;

                if (x1 != 0)
                {
                    previousPosition.v[0] = (2.0f * (static_cast<float>(fnvHash(seedX | (x1 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0f) * distance.v[0];
                    previousPosition.v[1] = (2.0f * (static_cast<float>(fnvHash(seedY | (x1 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0f) * distance.v[1];
                    previousPosition.v[2] = (2.0f * (static_cast<float>(fnvHash(seedZ | (x1 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0f) * distance.v[2];
                }

                if (x2 != static_cast<uint32_t>(timeScale))
                {
                    nextPosition.v[0] = (2.0f * (static_cast<float>(fnvHash(seedX | (x2 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0f) * distance.v[0];
                    nextPosition.v[1] = (2.0f * (static_cast<float>(fnvHash(seedY | (x2 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0f) * distance.v[1];
                    nextPosition.v[2] = (2.0f * (static_cast<float>(fnvHash(seedZ | (x2 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0f) * distance.v[2];
                }

                Vector3 noise(smoothStep(previousPosition.v[0], nextPosition.v[0], t),
                              smoothStep(previousPosition.v[1], nextPosition.v[1], t),
                              smoothStep(previousPosition.v[2], nextPosition.v[2], t));

                targetNode->setPosition(startPosition + noise);
            }
        }
    } // namespace scene
} // namespace ouzel
