// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Animator.h"
#include "core/Engine.h"
#include "scene/Node.h"

namespace ouzel
{
    namespace scene
    {
        Animator::Animator(float aLength):
            length(aLength)
        {
            updateCallback.callback = std::bind(&Animator::update, this, std::placeholders::_1);
        }

        Animator::~Animator()
        {
            for (const auto& animator : animators)
            {
                animator->parent = nullptr;
            }
        }

        void Animator::update(float delta)
        {
            if (running)
            {
                if (length == 0.0f) // never-ending action
                {
                    currentTime += delta;
                    progress = 0.0f;
                }
                else if (currentTime + delta >= length)
                {
                    done = true;
                    running = false;
                    progress = 1.0f;
                    currentTime = length;
                    if (finishHandler) finishHandler();
                }
                else
                {
                    currentTime += delta;
                    progress = currentTime / length;
                }

                updateProgress();
            }
            else
            {
                sharedEngine->unscheduleUpdate(&updateCallback);
            }
        }

        void Animator::start()
        {
            sharedEngine->scheduleUpdate(&updateCallback);
            play();
        }

        void Animator::play()
        {
            setProgress(0.0f);
            done = false;
            running = true;

            targetNode = node;

            if (!targetNode)
            {
                if (parent)
                {
                    targetNode = parent->getTargetNode();
                }
            }

            for (const auto& animator : animators)
            {
                animator->play();
            }
        }

        void Animator::resume()
        {
            running = true;
        }

        void Animator::stop(bool resetAnimation)
        {
            running = false;

            if (resetAnimation)
            {
                reset();
            }
        }

        void Animator::reset()
        {
            done = false;
            setProgress(0.0f);

            // reset animators in reverse order, so that Sequence is reset correctly
            for (auto i = animators.crbegin(); i != animators.crend(); ++i)
            {
                const std::shared_ptr<Animator>& animator = *i;
                animator->reset();
            }
        }

        void Animator::setProgress(float newProgress)
        {
            progress = newProgress;
            currentTime = progress * length;

            updateProgress();
        }

        void Animator::updateProgress()
        {
        }

        void Animator::addAnimator(const std::shared_ptr<Animator>& animator)
        {
            if (animator)
            {
                if (animator->parent)
                {
                    animator->parent->removeAnimator(animator);
                }

                animator->parent = this;

                animators.push_back(animator);
            }
        }

        bool Animator::removeAnimator(const std::shared_ptr<Animator>& animator)
        {
            for (auto i = animators.begin(); i != animators.end();)
            {
                if (*i == animator)
                {
                    animator->parent = nullptr;
                    animators.erase(i);
                    return true;
                }
                else
                {
                    ++i;
                }
            }

            return true;
        }
    } // namespace scene
} // namespace ouzel
