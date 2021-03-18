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
    namespace
    {
        float sineIn(const float t) noexcept
        {
            return 1.0F - std::cos(t * pi<float> / 2.0F);
        }

        float sineOut(const float t) noexcept
        {
            return std::sin(t * pi<float> / 2.0F);
        }

        float sineInOut(const float t) noexcept
        {
            return -0.5F * (std::cos(pi<float> * t) - 1.0F);
        }

        constexpr float quadIn(const float t) noexcept
        {
            return t * t;
        }

        constexpr float quadOut(const float t) noexcept
        {
            return t * (2.0F - t);
        }

        constexpr float quadInOut(const float t) noexcept
        {
            return (t < 0.5F) ?
                2.0F * t * t :
                -1.0F + (4.0F - 2.0F * t) * t;
        }

        constexpr float cubicIn(const float t) noexcept
        {
            return t * t * t;
        }

        constexpr float cubicOut(const float t) noexcept
        {
            return (t - 1.0F) * (t - 1.0F) * (t - 1.0F) + 1.0F;
        }

        constexpr float cubicInOut(const float t) noexcept
        {
            return (t < 0.5F) ?
                4.0F * t * t * t :
                (t - 1.0F) * (2.0F * t - 2.0F) * (2.0F * t - 2.0F) + 1.0F;
        }

        constexpr float quartIn(const float t) noexcept
        {
            return t * t * t * t;
        }

        constexpr float quartOut(const float t) noexcept
        {
            return 1.0F - (t - 1.0F) * (t - 1.0F) * (t - 1.0F);
        }

        constexpr float quartInOut(const float t) noexcept
        {
            return (t < 0.5F) ?
                8.0F * t * t * t * t :
                1.0F - 8.0F * (t - 1.0F) * (t - 1.0F) * (t - 1.0F) * (t - 1.0F);
        }

        constexpr float quintIn(const float t) noexcept
        {
            return t * t * t * t * t;
        }

        constexpr float quintOut(const float t) noexcept
        {
            return 1.0F + (t - 1.0F) * (t - 1.0F) * (t - 1.0F) * (t - 1.0F) * (t - 1.0F);
        }

        constexpr float quintInOut(const float t) noexcept
        {
            return (t < 0.5F) ?
                16.0F * t * t * t * t * t :
                1.0F + 16.0F * (t - 1.0F) * (t - 1.0F) * (t - 1.0F) * (t - 1.0F) * (t - 1.0F);
        }

        float expoIn(const float t) noexcept
        {
            return std::pow(2.0F, 10.0F * (t - 1.0F));
        }

        float expoOut(const float t) noexcept
        {
            return 1.0F - std::pow(2.0F, -10.0F * t);
        }

        float expoInOut(const float t) noexcept
        {
            return (t < 0.5F) ?
                0.5F * std::pow(2.0F, 10.0F * (2.0F * t - 1.0F)) :
                0.5F * (std::pow(2.0F, -10.0F * (t * 2.0F - 1.0F)) - 2.0F);
        }

        float circIn(const float t) noexcept
        {
            return 1.0F - std::sqrt(1.0F - t * t);
        }

        float circOut(const float t) noexcept
        {
            return std::sqrt(1.0F - (t - 1.0F) * (t - 1.0F));
        }

        float circInOut(const float t) noexcept
        {
            return (t < 0.5F) ?
                0.5F * (-std::sqrt(1.0F - (t * 2.0F) * (t * 2.0F)) + 1.0F) :
                0.5F * (std::sqrt(1.0F - (t * 2.0F - 2.0F) * (t * 2.0F - 2.0F)) + 1.0F);
        }

        float backIn(const float t) noexcept
        {
            static constexpr float s = 1.70158F;
            return t * t * ((s + 1.0F) * t - s);
        }

        float backOut(const float t) noexcept
        {
            static constexpr float s = 1.70158F;
            return (t - 1.0F) * (t - 1.0F) * ((s + 1.0F) * (t - 1.0F) + s) + 1.0F;
        }

        float backInOut(const float t) noexcept
        {
            static constexpr float s = 1.70158F * 1.525F;
            return (t < 0.5F) ?
                0.5F * ((t * 2.0F) * (t * 2.0F) * ((s + 1.0F) * (t * 2.0F) - s)):
                0.5F * ((t * 2.0F - 2.0F) * (t * 2.0F - 2.0F) * ((s + 1.0F) * (t * 2.0F - 2.0F) + s) + 2.0F);
        }

        float elasticIn(const float t) noexcept
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            static constexpr float p = 0.3F;

            return -std::pow(2.0F, 10.0F * (t - 1.0F)) * std::sin(((t - 1.0F) - p / 4.0F) * (2.0F * pi<float>) / p);
        }

        float elasticOut(const float t) noexcept
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            static constexpr float p = 0.3F;

            return std::pow(2.0F, -10.0F * t) * std::sin((t - p / 4.0F) * (2.0F * pi<float>) / p) + 1.0F;
        }

        float elasticInOut(const float t) noexcept
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            static constexpr float p = 0.3F * 1.5F;

            return (t < 0.5F) ?
                -0.5F * std::pow(2.0F, 10.0F * (t * 2.0F - 1.0F)) * std::sin(((t * 2.0F - 1.0F) - p / 4.0F) * (2.0F * pi<float>) / p) :
                0.5F * std::pow(2.0F, -10.0F * (t * 2.0F - 1.0F)) * std::sin(((t * 2.0F - 1.0F) - p / 4.0F) * (2.0F * pi<float>) / p) + 1.0F;
        }

        float bounceOut(const float t) noexcept
        {
            if (t < 1.0F / 2.75F)
                return 7.5625F * t * t;
            else if (t < 2.0F / 2.75F)
                return 7.5625F * (t - 1.5F / 2.75F) * (t - 1.5F / 2.75F) + 0.75F;
            else if (t < 2.5F / 2.75F)
                return 7.5625F * (t - 2.25F / 2.75F) * (t - 2.25F / 2.75F) + 0.9375F;
            else
                return 7.5625F * (t - 2.625F / 2.75F) * (t - 2.625F / 2.75F) + 0.984375F;
        }

        float bounceIn(const float t) noexcept
        {
            return 1.0F - bounceOut(1.0F - t);
        }

        float bounceInOut(const float t) noexcept
        {
            return (t < 0.5F) ?
                bounceOut(t * 2.0F) * 0.5F :
                bounceOut(t * 2.0F - 1.0F) * 0.5F + 0.5F;
        }
    }

    Ease::Ease(Animator& animator, Mode initMode, Func initFunc):
        Animator{animator.getLength()}, mode{initMode}, func{initFunc}
    {
        addAnimator(animator);
    }

    void Ease::updateProgress()
    {
        Animator::updateProgress();

        if (animators.empty()) return;

        switch (mode)
        {
            case Mode::easeIn:
            {
                switch (func)
                {
                    case Func::sine: progress = sineIn(progress); break;
                    case Func::quad: progress = quadIn(progress); break;
                    case Func::cubic: progress = cubicIn(progress); break;
                    case Func::quart: progress = quartIn(progress); break;
                    case Func::quint: progress = quintIn(progress); break;
                    case Func::expo: progress = expoIn(progress); break;
                    case Func::circ: progress = circIn(progress); break;
                    case Func::back: progress = backIn(progress); break;
                    case Func::elastic: progress = elasticIn(progress); break;
                    case Func::bounce: progress = bounceIn(progress); break;
                    default: throw std::runtime_error("Invalid function");
                }
                break;
            }

            case Mode::easeOut:
            {
                switch (func)
                {
                    case Func::sine: progress = sineOut(progress); break;
                    case Func::quad: progress = quadOut(progress); break;
                    case Func::cubic: progress = cubicOut(progress); break;
                    case Func::quart: progress = quartOut(progress); break;
                    case Func::quint: progress = quintOut(progress); break;
                    case Func::expo: progress = expoOut(progress); break;
                    case Func::circ: progress = circOut(progress); break;
                    case Func::back: progress = backOut(progress); break;
                    case Func::elastic: progress = elasticOut(progress); break;
                    case Func::bounce: progress = bounceOut(progress); break;
                    default: throw std::runtime_error("Invalid function");
                }
                break;
            }

            case Mode::easeInOut:
            {
                switch (func)
                {
                    case Func::sine: progress = sineInOut(progress); break;
                    case Func::quad: progress = quadInOut(progress); break;
                    case Func::cubic: progress = cubicInOut(progress); break;
                    case Func::quart: progress = quartInOut(progress); break;
                    case Func::quint: progress = quintInOut(progress); break;
                    case Func::expo: progress = expoInOut(progress); break;
                    case Func::circ: progress = circInOut(progress); break;
                    case Func::back: progress = backInOut(progress); break;
                    case Func::elastic: progress = elasticInOut(progress); break;
                    case Func::bounce: progress = bounceInOut(progress); break;
                    default: throw std::runtime_error("Invalid function");
                }
                break;
            }

            default: throw std::runtime_error("Invalid mode");
        }

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

    Move::Move(float initLength, const Vector<float, 3>& initPosition, bool initRelative):
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

    Rotate::Rotate(float initLength, const Vector<float, 3>& initRotation, bool initRelative):
        Animator(initLength), rotation(initRotation), relative(initRelative)
    {
    }

    void Rotate::play()
    {
        Animator::play();

        if (targetActor)
        {
            startRotation = targetActor->getRotation().getEulerAngles();

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

    Scale::Scale(float initLength, const Vector<float, 3>& initScale, bool initRelative):
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

    Shake::Shake(float initLength, const Vector<float, 3>& initDistance, float initTimeScale):
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

            Vector<float, 3> previousPosition;
            Vector<float, 3> nextPosition;

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

            const Vector<float, 3> noise{
                smoothStep(previousPosition.v[0], nextPosition.v[0], t),
                smoothStep(previousPosition.v[1], nextPosition.v[1], t),
                smoothStep(previousPosition.v[2], nextPosition.v[2], t)
            };

            targetActor->setPosition(startPosition + noise);
        }
    }
}
