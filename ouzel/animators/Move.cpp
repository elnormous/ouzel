// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Move.hpp"
#include "scene/Node.hpp"

namespace ouzel
{
    namespace scene
    {
        Move::Move(float aLength, const Vector3& aPosition, bool aRelative):
            Animator(aLength), position(aPosition), relative(aRelative)
        {
        }

        void Move::play()
        {
            Animator::play();

            if (targetNode)
            {
                startPosition = targetNode->getPosition();
                targetPosition = relative ? startPosition + position : position;

                diff = targetPosition - startPosition;
            }
        }

        void Move::updateProgress()
        {
            Animator::updateProgress();

            if (targetNode)
            {
                targetNode->setPosition(startPosition + (diff * progress));
            }
        }
    } // namespace scene
} // namespace ouzel
