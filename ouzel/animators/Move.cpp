// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Move.hpp"
#include "scene/Actor.hpp"

namespace ouzel
{
    namespace scene
    {
        Move::Move(float initLength, const Vector3& initPosition, bool initRelative):
            Animator(initLength), position(initPosition), relative(initRelative)
        {
        }

        void Move::play()
        {
            Animator::play();

            if (targetActor)
            {
                startPosition = targetActor->getPosition();
                targetPosition = relative ? startPosition + position : position;

                diff = targetPosition - startPosition;
            }
        }

        void Move::updateProgress()
        {
            Animator::updateProgress();

            if (targetActor)
                targetActor->setPosition(startPosition + (diff * progress));
        }
    } // namespace scene
} // namespace ouzel
