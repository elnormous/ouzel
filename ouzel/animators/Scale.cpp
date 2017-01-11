// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Scale.h"
#include "scene/Node.h"

namespace ouzel
{
    namespace scene
    {
        Scale::Scale(float aLength, const Vector3& aScale, bool aRelative):
            Animator(aLength), scale(aScale), relative(aRelative)
        {
        }

        void Scale::start(Node* newTargetNode)
        {
            Animator::start(newTargetNode);

            if (targetNode)
            {
                startScale = targetNode->getScale();
                targetScale = relative ? startScale + scale : scale;

                diff = targetScale - startScale;
            }
        }

        void Scale::updateProgress()
        {
            Animator::updateProgress();

            if (targetNode)
            {
                targetNode->setScale(startScale + (diff * progress));
            }
        }
    } // namespace scene
} // namespace ouzel
