// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Fade.h"
#include "Node.h"

namespace ouzel
{
    namespace scene
    {
        Fade::Fade(float length, float rotation, bool relative):
            Animator(length), _opacity(rotation), _relative(relative)
        {

        }

        void Fade::start(const NodePtr& node)
        {
            Animator::start(node);

            if (node)
            {
                _startOpacity = node->getOpacity();
                _targetOpacity = _relative ? _startOpacity + _opacity : _opacity;

                _diff = _targetOpacity - _startOpacity;
            }
        }

        void Fade::setProgress(float progress)
        {
            Animator::setProgress(progress);

            if (NodePtr node = _node.lock())
            {
                node->setOpacity(_startOpacity + (_diff * _progress));
            }
        }
    } // namespace scene
} // namespace ouzel
