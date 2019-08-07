// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cmath>
#include <limits>
#include "Animators.hpp"
#include "Actor.hpp"
#include "core/Engine.hpp"
#include "math/Fnv.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        static float sineIn(const float t)
        {
            return 1.0F - cos(t * pi<float> / 2.0F);
        }

        static float sineOut(const float t)
        {
            return sin(t * pi<float> / 2.0F);
        }

        static float sineInOut(const float t)
        {
            return -0.5F * (cos(pi<float> * t) - 1.0F);
        }

        static constexpr float quadIn(const float t)
        {
            return t * t;
        }

        static constexpr float quadOut(const float t)
        {
            return t * (2.0F - t);
        }

        static constexpr float quadInOut(const float t)
        {
            return (t < 0.5F) ?
                2.0F * t * t :
                -1.0F + (4.0F - 2.0F * t) * t;
        }

        static constexpr float cubicIn(const float t)
        {
            return t * t * t;
        }

        static constexpr float cubicOut(const float t)
        {
            return (t - 1.0F) * (t - 1.0F) * (t - 1.0F) + 1.0F;
        }

        static constexpr float cubicInOut(const float t)
        {
            return (t < 0.5F) ?
                4.0F * t * t * t :
                (t - 1.0F) * (2.0F * t - 2.0F) * (2.0F * t - 2.0F) + 1.0F;
        }

        static constexpr float quartIn(const float t)
        {
            return t * t * t * t;
        }

        static constexpr float quartOut(const float t)
        {
            return 1.0F - (t - 1.0F) * (t - 1.0F) * (t - 1.0F);
        }

        static constexpr float quartInOut(const float t)
        {
            return (t < 0.5F) ?
                8.0F * t * t * t * t :
                1.0F - 8.0F * (t - 1.0F) * (t - 1.0F) * (t - 1.0F) * (t - 1.0F);
        }

        static constexpr float quintIn(const float t)
        {
            return t * t * t * t * t;
        }

        static constexpr float quintOut(const float t)
        {
            return 1.0F + (t - 1.0F) * (t - 1.0F) * (t - 1.0F) * (t - 1.0F) * (t - 1.0F);
        }

        static constexpr float quintInOut(const float t)
        {
            return (t < 0.5F) ?
                16.0F * t * t * t * t * t :
                1.0F + 16.0F * (t - 1.0F) * (t - 1.0F) * (t - 1.0F) * (t - 1.0F) * (t - 1.0F);
        }

        static float expoIn(const float t)
        {
            return pow(2.0F, 10.0F * (t - 1.0F));
        }

        static float expoOut(const float t)
        {
            return 1.0F - pow(2.0F, -10.0F * t);
        }

        static float expoInOut(const float t)
        {
            return (t < 0.5F) ?
                0.5F * pow(2.0F, 10.0F * (2.0F * t - 1.0F)) :
                0.5F * (pow(2.0F, -10.0F * (t * 2.0F - 1.0F)) - 2.0F);
        }

        static float circIn(const float t)
        {
            return 1.0F - sqrt(1.0F - t * t);
        }

        static float circOut(const float t)
        {
            return sqrt(1.0F - (t - 1.0F) * (t - 1.0F));
        }

        static float circInOut(const float t)
        {
            return (t < 0.5F) ?
                0.5F * (-sqrt(1.0F - (t * 2.0F) * (t * 2.0F)) + 1.0F) :
                0.5F * (sqrt(1.0F - (t * 2.0F - 2.0F) * (t * 2.0F - 2.0F)) + 1.0F);
        }

        static float backIn(const float t)
        {
            static constexpr float s = 1.70158F;
            return t * t * ((s + 1.0F) * t - s);
        }

        static float backOut(const float t)
        {
            static constexpr float s = 1.70158F;
            return (t - 1.0F) * (t - 1.0F) * ((s + 1.0F) * (t - 1.0F) + s) + 1.0F;
        }

        static float backInOut(const float t)
        {
            static constexpr float s = 1.70158F * 1.525F;
            return (t < 0.5F) ?
                0.5F * ((t * 2.0F) * (t * 2.0F) * ((s + 1.0F) * (t * 2.0F) - s)):
                0.5F * ((t * 2.0F - 2.0F) * (t * 2.0F - 2.0F) * ((s + 1.0F) * (t * 2.0F - 2.0F) + s) + 2.0F);
        }

        static float elasticIn(const float t)
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            static constexpr float p = 0.3F;

            return -pow(2.0F, 10.0F * (t - 1.0F)) * sin(((t - 1.0F) - p / 4.0F) * (2.0F * pi<float>) / p);
        }

        static float elasticOut(const float t)
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            static constexpr float p = 0.3F;

            return pow(2.0F, -10.0F * t) * sin((t - p / 4.0F) * (2.0F * pi<float>) / p) + 1.0F;
        }

        static float elasticInOut(const float t)
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            static constexpr float p = 0.3F * 1.5F;

            return (t < 0.5F) ?
                -0.5F * pow(2.0F, 10.0F * (t * 2.0F - 1.0F)) * sin(((t * 2.0F - 1.0F) - p / 4.0F) * (2.0F * pi<float>) / p) :
                0.5F * pow(2.0F, -10.0F * (t * 2.0F - 1.0F)) * sin(((t * 2.0F - 1.0F) - p / 4.0F) * (2.0F * pi<float>) / p) + 1.0F;
        }

        static float bounceOut(const float t)
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

        static float bounceIn(const float t)
        {
            return 1.0F - bounceOut(1.0F - t);
        }

        static float bounceInOut(const float t)
        {
            return (t < 0.5F) ?
                bounceOut(t * 2.0F) * 0.5F :
                bounceOut(t * 2.0F - 1.0F) * 0.5F + 0.5F;
        }

        Ease::Ease(Animator& animator, Mode initMode, Func initFunc):
            Animator(animator.getLength()), mode(initMode), func(initFunc)
        {
            addAnimator(&animator);
        }

        void Ease::updateProgress()
        {
            Animator::updateProgress();

            if (animators.empty()) return;

            switch (mode)
            {
                case Mode::EaseIn:
                {
                    switch (func)
                    {
                        case Func::Sine: progress = sineIn(progress); break;
                        case Func::Quad: progress = quadIn(progress); break;
                        case Func::Cubic: progress = cubicIn(progress); break;
                        case Func::Quart: progress = quartIn(progress); break;
                        case Func::Quint: progress = quintIn(progress); break;
                        case Func::Expo: progress = expoIn(progress); break;
                        case Func::Circ: progress = circIn(progress); break;
                        case Func::Back: progress = backIn(progress); break;
                        case Func::Elastic: progress = elasticIn(progress); break;
                        case Func::Bounce: progress = bounceIn(progress); break;
                        default: return;
                    }
                    break;
                }

                case Mode::EaseOut:
                {
                    switch (func)
                    {
                        case Func::Sine: progress = sineOut(progress); break;
                        case Func::Quad: progress = quadOut(progress); break;
                        case Func::Cubic: progress = cubicOut(progress); break;
                        case Func::Quart: progress = quartOut(progress); break;
                        case Func::Quint: progress = quintOut(progress); break;
                        case Func::Expo: progress = expoOut(progress); break;
                        case Func::Circ: progress = circOut(progress); break;
                        case Func::Back: progress = backOut(progress); break;
                        case Func::Elastic: progress = elasticOut(progress); break;
                        case Func::Bounce: progress = bounceOut(progress); break;
                        default: return;
                    }
                    break;
                }

                case Mode::EaseInOut:
                {
                    switch (func)
                    {
                        case Func::Sine: progress = sineInOut(progress); break;
                        case Func::Quad: progress = quadInOut(progress); break;
                        case Func::Cubic: progress = cubicInOut(progress); break;
                        case Func::Quart: progress = quartInOut(progress); break;
                        case Func::Quint: progress = quintInOut(progress); break;
                        case Func::Expo: progress = expoInOut(progress); break;
                        case Func::Circ: progress = circInOut(progress); break;
                        case Func::Back: progress = backInOut(progress); break;
                        case Func::Elastic: progress = elasticInOut(progress); break;
                        case Func::Bounce: progress = bounceInOut(progress); break;
                        default: return;
                    }
                    break;
                }

                default: return;
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

        Move::Move(float initLength, const Vector3F& initPosition, bool initRelative):
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
            for (Animator* animator : initAnimators)
            {
                addAnimator(animator);

                if (animator->getLength() > length)
                    length = animator->getLength();
            }
        }

        Parallel::Parallel(const std::vector<std::unique_ptr<Animator>>& initAnimators):
            Animator(0.0F)
        {
            for (const std::unique_ptr<Animator>& animator : initAnimators)
            {
                addAnimator(animator.get());

                if (animator->getLength() > length)
                    length = animator->getLength();
            }
        }

        void Parallel::updateProgress()
        {
            Animator::updateProgress();

            for (Animator* animator : animators)
            {
                const float animationLength = animator->getLength();

                if (animationLength <= 0.0F || currentTime > animationLength)
                    animator->setProgress(1.0F);
                else
                    animator->setProgress(currentTime / animationLength);
            }
        }

        Repeat::Repeat(Animator& animator, uint32_t initCount):
            Animator(animator.getLength() * static_cast<float>(initCount)), count(initCount)
        {
            addAnimator(&animator);
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
                currentCount = static_cast<uint32_t>(currentTime / animators.front()->getLength());

                if (count == 0 || currentCount < count)
                {
                    done = false;
                    running = true;

                    const float remainingTime = currentTime - animators.front()->getLength() * static_cast<float>(currentCount);
                    animators.front()->setProgress(remainingTime / animators.front()->getLength());

                    auto resetEvent = std::make_unique<AnimationEvent>();
                    resetEvent->type = Event::Type::AnimationReset;
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
                    finishEvent->type = Event::Type::AnimationFinish;
                    finishEvent->component = this;
                    engine->getEventDispatcher().dispatchEvent(std::move(finishEvent));
                }
            }
        }

        Rotate::Rotate(float initLength, const Vector3F& initRotation, bool initRelative):
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

        Scale::Scale(float initLength, const Vector3F& initScale, bool initRelative):
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
            Animator(std::accumulate(initAnimators.begin(), initAnimators.end(), 0.0F, [](float a, Animator* b) { return a + b->getLength(); }))
        {
            for (Animator* animator : initAnimators)
                addAnimator(animator);
        }

        Sequence::Sequence(const std::vector<std::unique_ptr<Animator>>& initAnimators):
            Animator(std::accumulate(initAnimators.begin(), initAnimators.end(), 0.0F, [](float a, const std::unique_ptr<Animator>& b) { return a + b->getLength(); }))
        {
            for (const std::unique_ptr<Animator>& animator : initAnimators)
                addAnimator(animator.get());
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

            for (Animator* animator : animators)
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

        Shake::Shake(float initLength, const Vector3F& initDistance, float initTimeScale):
            Animator(initLength), distance(initDistance), timeScale(initTimeScale)
        {
            seedX = std::uniform_int_distribution<uint32_t>{0, std::numeric_limits<uint32_t>::max()}(randomEngine);
            seedY = std::uniform_int_distribution<uint32_t>{0, std::numeric_limits<uint32_t>::max()}(randomEngine);
            seedZ = std::uniform_int_distribution<uint32_t>{0, std::numeric_limits<uint32_t>::max()}(randomEngine);
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

                const auto x1 = static_cast<uint64_t>(x);
                const auto x2 = x1 + 1;
                const auto t = x - static_cast<float>(x1);

                Vector3F previousPosition;
                Vector3F nextPosition;

                if (x1 != 0)
                {
                    previousPosition.v[0] = (2.0F * (static_cast<float>(fnv::hash<uint32_t>(seedX | (x1 << 32))) / std::numeric_limits<uint32_t>::max()) - 1.0F) * distance.v[0];
                    previousPosition.v[1] = (2.0F * (static_cast<float>(fnv::hash<uint32_t>(seedY | (x1 << 32))) / std::numeric_limits<uint32_t>::max()) - 1.0F) * distance.v[1];
                    previousPosition.v[2] = (2.0F * (static_cast<float>(fnv::hash<uint32_t>(seedZ | (x1 << 32))) / std::numeric_limits<uint32_t>::max()) - 1.0F) * distance.v[2];
                }

                if (x2 != static_cast<uint32_t>(timeScale))
                {
                    nextPosition.v[0] = (2.0F * (static_cast<float>(fnv::hash<uint32_t>(seedX | (x2 << 32))) / std::numeric_limits<uint32_t>::max()) - 1.0F) * distance.v[0];
                    nextPosition.v[1] = (2.0F * (static_cast<float>(fnv::hash<uint32_t>(seedY | (x2 << 32))) / std::numeric_limits<uint32_t>::max()) - 1.0F) * distance.v[1];
                    nextPosition.v[2] = (2.0F * (static_cast<float>(fnv::hash<uint32_t>(seedZ | (x2 << 32))) / std::numeric_limits<uint32_t>::max()) - 1.0F) * distance.v[2];
                }

                const Vector3F noise(smoothStep(previousPosition.v[0], nextPosition.v[0], t),
                                     smoothStep(previousPosition.v[1], nextPosition.v[1], t),
                                     smoothStep(previousPosition.v[2], nextPosition.v[2], t));

                targetActor->setPosition(startPosition + noise);
            }
        }
    } // namespace scene
} // namespace ouzel
