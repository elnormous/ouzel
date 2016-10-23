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
        Shake::Shake(float aLength, const Vector2& aDistance, float aTimeScale):
            Animator(aLength), distance(aDistance), timeScale(aTimeScale)
        {
            seedX = random();
            seedY = random();
        }

        void Shake::start(Node* targetNode)
        {
            Animator::start(targetNode);

            if (targetNode)
            {
                startPosition = targetNode->getPosition();
            }
        }

        void Shake::updateProgress()
        {
            Animator::updateProgress();

            if (node)
            {
                float x = length * progress * timeScale;

                uint64_t x1 = static_cast<uint32_t>(x);
                uint64_t x2 = x1 + 1;
                float t = x - static_cast<float>(x1);

                Vector2 previousPosition;
                Vector2 nextPosition;

                if (x1 != 0)
                {
                    previousPosition.x = (2.0f * (static_cast<float>(fnvHash(seedX | (x1 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0f) * distance.x;
                    previousPosition.y = (2.0f * (static_cast<float>(fnvHash(seedY | (x1 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0f) * distance.y;
                }

                if (x2 != static_cast<uint32_t>(timeScale))
                {
                    nextPosition.x = (2.0f * (static_cast<float>(fnvHash(seedX | (x2 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0f) * distance.x;
                    nextPosition.y = (2.0f * (static_cast<float>(fnvHash(seedY | (x2 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0f) * distance.y;
                }

                Vector2 noise(smoothstep(previousPosition.x, nextPosition.x, t),
                              smoothstep(previousPosition.y, nextPosition.y, t));

                node->setPosition(startPosition + noise);
            }
        }
    } // namespace scene
} // namespace ouzel
