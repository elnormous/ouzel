// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Animator.hpp"
#include "core/Engine.hpp"
#include "scene/Node.hpp"

namespace ouzel
{
    namespace scene
    {
        Animator::Animator(float aLength):
            Component(Component::ANIMATOR), length(aLength)
        {
            updateCallback.callback = std::bind(&Animator::update, this, std::placeholders::_1);
        }

        Animator::~Animator()
        {
            if (parent) parent->removeAnimator(this);

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

                    Event finishEvent;
                    finishEvent.type = Event::Type::ANIMATION_FINISH;
                    finishEvent.animationEvent.component = this;
                    sharedEngine->getEventDispatcher()->postEvent(finishEvent);
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
                updateCallback.remove();
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
                Animator* animator = *i;
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

        void Animator::addChildAnimator(Animator* animator)
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

        bool Animator::removeChildAnimator(Animator* animator)
        {
            bool result = false;

            std::vector<Animator*>::iterator animatorIterator = std::find(animators.begin(), animators.end(), animator);

            if (animatorIterator != animators.end())
            {
                animator->parent = nullptr;
                animators.erase(animatorIterator);
                result = true;
            }

            std::vector<std::unique_ptr<Animator>>::iterator ownedIterator = std::find_if(ownedAnimators.begin(), ownedAnimators.end(), [animator](const std::unique_ptr<Animator>& other) {
                return other.get() == animator;
            });

            if (ownedIterator != ownedAnimators.end())
            {
                ownedAnimators.erase(ownedIterator);
            }

            return result;
        }

        void Animator::removeAllAnimators()
        {
            animators.clear();
            ownedAnimators.clear();
        }

        void Animator::removeFromParent()
        {
            if (parent) parent->removeAnimator(this);
        }
    } // namespace scene
} // namespace ouzel
