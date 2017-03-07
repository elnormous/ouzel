// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Move.h"
#include "scene/Node.h"

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

            if (std::shared_ptr<Node> node = targetNode.lock())
            {
                startPosition = node->getPosition();
                targetPosition = relative ? startPosition + position : position;

                diff = targetPosition - startPosition;
            }
        }

        void Move::updateProgress()
        {
            Animator::updateProgress();

            if (std::shared_ptr<Node> node = targetNode.lock())
            {
                node->setPosition(startPosition + (diff * progress));
            }
        }
    } // namespace scene
} // namespace ouzel
