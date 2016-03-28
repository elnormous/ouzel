// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Repeat.h"

namespace ouzel
{
    namespace scene
    {
        Repeat::Repeat(const AnimatorPtr& animator, uint32_t count):
            Animator(animator->getLength() * static_cast<float>(count)), _animator(animator), _count(count)
        {

        }

        void Repeat::update(float delta)
        {
            if (_running)
            {
                if (!_animator || _animator->getLength() <= 0.0f)
                {
                    _currentCount = _count;
                    _done = true;
                    _running = false;
                    _currentTime = _length;
                    setProgress(1.0f);
                }
                else
                {
                    _currentTime += delta;
                    _currentCount = static_cast<uint32_t>(_currentTime / _animator->getLength());

                    if (_count == 0 || _currentCount < _count)
                    {
                        float remainingTime = _currentTime - _animator->getLength() * _currentCount;
                        _animator->setProgress(remainingTime / _animator->getLength());
                    }
                    else
                    {
                        _done = true;
                        _running = false;
                        _currentTime = _length;
                        setProgress(1.0f);
                    }
                }
            }
        }

        void Repeat::start(const NodePtr& node)
        {
            Animator::start(node);

            if (_animator)
            {
                _animator->start(node);
            }
        }

        void Repeat::reset()
        {
            Animator::reset();

            if (_animator)
            {
                _animator->reset();
            }

            _currentCount = 0;
        }
    } // namespace scene
} // namespace ouzel
