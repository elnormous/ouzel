// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Scale.h"
#include "scene/Node.h"

namespace ouzel
{
    namespace scene
    {
        Scale::Scale(float aLength, const Vector2& aScale, bool aRelative):
            Animator(aLength), scale(aScale), relative(aRelative)
        {
        }

        void Scale::start(Node* targetNode)
        {
            Animator::start(targetNode);

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

            if (node)
            {
                node->setScale(startScale + (diff * progress));
            }
        }
    } // namespace scene
} // namespace ouzel
