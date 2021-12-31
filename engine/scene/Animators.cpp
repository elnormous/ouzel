// Ouzel by Elviss Strazdins

#include <cmath>
#include <limits>
#include "Animators.hpp"
#include "Actor.hpp"
#include "../core/Engine.hpp"
#include "../hash/Fnv1.hpp"
#include "../utils/Utils.hpp"

namespace ouzel::scene
{
    Ease::Ease(Animator& animator, easing::Func initFunc, easing::Mode initMode):
        Animator{animator.getLength()}, func{initFunc}, mode{initMode}
    {
        addAnimator(animator);
    }

    void Ease::updateProgress()
    {
        Animator::updateProgress();

        if (animators.empty()) return;

        progress = easing::ease(func, mode, progress);

        animators.front()->setProgress(progress);
    }

    Fade::Fade(float initLength, float initOpacity, bool initRelative):
        Animator(initLength), opacity(initOpacity), relative(initRelative)
    {
    }

    void Fade::play()
    {
        Animator::play();

        if (targetActor)
        {
            startOpacity = targetActor->getOpacity();
            targetOpacity = relative ? startOpacity + opacity : opacity;

            diff = targetOpacity - startOpacity;
        }
    }

    void Fade::updateProgress()
    {
        Animator::updateProgress();

        if (targetActor)
            targetActor->setOpacity(startOpacity + (diff * progress));
    }

    Move::Move(float initLength, const math::Vector<float, 3>& initPosition, bool initRelative):
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

    Parallel::Parallel(const std::vector<Animator*>& initAnimators):
        Animator(0.0F)
    {
        for (auto animator : initAnimators)
        {
            assert(animator);

            addAnimator(*animator);

            if (animator->getLength() > length)
                length = animator->getLength();
        }
    }

    Parallel::Parallel(const std::vector<std::unique_ptr<Animator>>& initAnimators):
        Animator(0.0F)
    {
        for (const std::unique_ptr<Animator>& animator : initAnimators)
        {
            assert(animator.get());

            addAnimator(*animator);

            if (animator->getLength() > length)
                length = animator->getLength();
        }
    }

    void Parallel::updateProgress()
    {
        Animator::updateProgress();

        for (auto animator : animators)
        {
            const float animationLength = animator->getLength();

            if (animationLength <= 0.0F || currentTime > animationLength)
                animator->setProgress(1.0F);
            else
                animator->setProgress(currentTime / animationLength);
        }
    }

    Repeat::Repeat(Animator& animator, std::uint32_t initCount):
        Animator(animator.getLength() * static_cast<float>(initCount)), count(initCount)
    {
        addAnimator(animator);
    }

    void Repeat::reset()
    {
        Animator::reset();

        currentCount = 0;
    }

    void Repeat::updateProgress()
    {
        if (animators.empty()) return;

        if (animators.front()->getLength() != 0.0F)
        {
            currentCount = static_cast<std::uint32_t>(currentTime / animators.front()->getLength());

            if (count == 0 || currentCount < count)
            {
                done = false;
                running = true;

                const float remainingTime = currentTime - animators.front()->getLength() * static_cast<float>(currentCount);
                animators.front()->setProgress(remainingTime / animators.front()->getLength());

                auto resetEvent = std::make_unique<AnimationEvent>();
                resetEvent->type = Event::Type::animationReset;
                resetEvent->component = this;
                engine->getEventDispatcher().dispatchEvent(std::move(resetEvent));
            }
            else
            {
                done = true;
                running = false;
                currentTime = length;
                progress = 1.0F;

                auto finishEvent = std::make_unique<AnimationEvent>();
                finishEvent->type = Event::Type::animationFinish;
                finishEvent->component = this;
                engine->getEventDispatcher().dispatchEvent(std::move(finishEvent));
            }
        }
    }

    Rotate::Rotate(float initLength, const math::Vector<float, 3>& initRotation, bool initRelative):
        Animator(initLength), rotation(initRotation), relative(initRelative)
    {
    }

    void Rotate::play()
    {
        Animator::play();

        if (targetActor)
        {
            startRotation = getEulerAngles(targetActor->getRotation());

            targetRotation = relative ? startRotation + rotation : rotation;

            diff = targetRotation - startRotation;
        }
    }

    void Rotate::updateProgress()
    {
        Animator::updateProgress();

        if (targetActor)
            targetActor->setRotation(startRotation + diff * progress);
    }

    Scale::Scale(float initLength, const math::Vector<float, 3>& initScale, bool initRelative):
        Animator(initLength), scale(initScale), relative(initRelative)
    {
    }

    void Scale::play()
    {
        Animator::play();

        if (targetActor)
        {
            startScale = targetActor->getScale();
            targetScale = relative ? startScale + scale : scale;

            diff = targetScale - startScale;
        }
    }

    void Scale::updateProgress()
    {
        Animator::updateProgress();

        if (targetActor)
            targetActor->setScale(startScale + (diff * progress));
    }

    Sequence::Sequence(const std::vector<Animator*>& initAnimators):
        Animator(std::accumulate(initAnimators.begin(), initAnimators.end(), 0.0F, [](float a, Animator* b) noexcept { return a + b->getLength(); }))
    {
        for (auto animator : initAnimators)
        {
            assert(animator);

            addAnimator(*animator);
        }
    }

    Sequence::Sequence(const std::vector<std::unique_ptr<Animator>>& initAnimators):
        Animator(std::accumulate(initAnimators.begin(), initAnimators.end(), 0.0F, [](float a, const std::unique_ptr<Animator>& b) noexcept { return a + b->getLength(); }))
    {
        for (const std::unique_ptr<Animator>& animator : initAnimators)
        {
            assert(animator.get());

            addAnimator(*animator);
        }
    }

    void Sequence::play()
    {
        setProgress(0.0F);
        done = false;
        running = true;

        targetActor = actor;

        if (!targetActor && parent)
            targetActor = parent->getTargetActor();

        if (!animators.empty())
        {
            currentAnimator = animators.front();
            currentAnimator->play();
        }
        else
            currentAnimator = nullptr;
    }

    void Sequence::updateProgress()
    {
        Animator::updateProgress();

        float time = 0.0F;

        for (auto animator : animators)
        {
            if (animator->getLength() <= 0.0F || currentTime > time + animator->getLength())
            {
                if (animator == currentAnimator) animator->setProgress(1.0F);
            }
            else if (currentTime <= time)
            {
                if (animator == currentAnimator) animator->setProgress(0.0F);
            }
            else
            {
                if (currentAnimator != animator)
                {
                    currentAnimator = animator;
                    animator->play();
                }

                animator->setProgress((currentTime - time) / animator->getLength());
            }

            time += animator->getLength();
        }
    }

    Shake::Shake(float initLength, const math::Vector<float, 3>& initDistance, float initTimeScale):
        Animator(initLength), distance(initDistance), timeScale(initTimeScale)
    {
        seedX = std::uniform_int_distribution<std::uint32_t>{0, std::numeric_limits<std::uint32_t>::max()}(core::randomEngine);
        seedY = std::uniform_int_distribution<std::uint32_t>{0, std::numeric_limits<std::uint32_t>::max()}(core::randomEngine);
        seedZ = std::uniform_int_distribution<std::uint32_t>{0, std::numeric_limits<std::uint32_t>::max()}(core::randomEngine);
    }

    void Shake::play()
    {
        Animator::play();

        if (targetActor)
            startPosition = targetActor->getPosition();
    }

    void Shake::updateProgress()
    {
        Animator::updateProgress();

        if (targetActor)
        {
            const float x = length * progress * timeScale;

            const auto x1 = static_cast<std::uint64_t>(x);
            const auto x2 = x1 + 1;
            const auto t = x - static_cast<float>(x1);

            math::Vector<float, 3> previousPosition{};
            math::Vector<float, 3> nextPosition{};

            if (x1 != 0)
            {
                previousPosition.v[0] = (2.0F * (static_cast<float>(hash::fnv1::hash<std::uint32_t>(seedX | (x1 << 32))) / std::numeric_limits<std::uint32_t>::max()) - 1.0F) * distance.v[0];
                previousPosition.v[1] = (2.0F * (static_cast<float>(hash::fnv1::hash<std::uint32_t>(seedY | (x1 << 32))) / std::numeric_limits<std::uint32_t>::max()) - 1.0F) * distance.v[1];
                previousPosition.v[2] = (2.0F * (static_cast<float>(hash::fnv1::hash<std::uint32_t>(seedZ | (x1 << 32))) / std::numeric_limits<std::uint32_t>::max()) - 1.0F) * distance.v[2];
            }

            if (x2 != static_cast<std::uint32_t>(timeScale))
            {
                nextPosition.v[0] = (2.0F * (static_cast<float>(hash::fnv1::hash<std::uint32_t>(seedX | (x2 << 32))) / std::numeric_limits<std::uint32_t>::max()) - 1.0F) * distance.v[0];
                nextPosition.v[1] = (2.0F * (static_cast<float>(hash::fnv1::hash<std::uint32_t>(seedY | (x2 << 32))) / std::numeric_limits<std::uint32_t>::max()) - 1.0F) * distance.v[1];
                nextPosition.v[2] = (2.0F * (static_cast<float>(hash::fnv1::hash<std::uint32_t>(seedZ | (x2 << 32))) / std::numeric_limits<std::uint32_t>::max()) - 1.0F) * distance.v[2];
            }

            const math::Vector<float, 3> noise{
                math::smoothStep(previousPosition.v[0], nextPosition.v[0], t),
                math::smoothStep(previousPosition.v[1], nextPosition.v[1], t),
                math::smoothStep(previousPosition.v[2], nextPosition.v[2], t)
            };

            targetActor->setPosition(startPosition + noise);
        }
    }
}
