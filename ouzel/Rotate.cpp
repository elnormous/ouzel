// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Rotate.h"
#include "Node.h"

namespace ouzel
{
    namespace scene
    {
        Rotate::Rotate(float pLength, float pRotation, bool pRelative):
            Animator(pLength), rotation(pRotation), relative(pRelative)
        {

        }

        void Rotate::start(const NodePtr& targetNode)
        {
            Animator::start(targetNode);

            if (targetNode)
            {
                startRotation = targetNode->getRotation();
                targetRotation = relative ? startRotation + rotation : rotation;

                diff = targetRotation - startRotation;
            }
        }

        void Rotate::updateProgress()
        {
            Animator::updateProgress();

            if (NodePtr targetNode = node.lock())
            {
                targetNode->setRotation(startRotation + (diff * progress));
            }
        }
    } // namespace scene
} // namespace ouzel
