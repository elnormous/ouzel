// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cmath>
#include <limits>
#include "Animators.hpp"
#include "Actor.hpp"
#include "core/Engine.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        static float sineIn(float t)
        {
            return 1.0F - cos(t * pi<float> / 2.0F);
        }

        static float sineOut(float t)
        {
            return sin(t * pi<float> / 2.0F);
        }

        static float sineInOut(float t)
        {
            return -0.5F * (cos(pi<float> * t) - 1.0F);
        }

        static float quadIn(float t)
        {
            return t * t;
        }

        static float quadOut(float t)
        {
            return t * (2.0F - t);
        }

        static float quadInOut(float t)
        {
            if (t < 0.5F)
                return 2.0F * t * t;
            else
                return -1.0F + (4.0F - 2.0F * t) * t;
        }

        static float cubicIn(float t)
        {
            return t * t * t;
        }

        static float cubicOut(float t)
        {
            t -= 1.0F;
            return t * t * t + 1.0F;
        }

        static float cubicInOut(float t)
        {
            if (t < 0.5F)
                return 4.0F * t * t * t;
            else
                return (t - 1.0F) * (2.0F * t - 2.0F) * (2.0F * t - 2.0F) + 1.0F;
        }

        static float quartIn(float t)
        {
            return t * t * t * t;
        }

        static float quartOut(float t)
        {
            t -= 1.0F;
            return 1.0F - t * t * t;
        }

        static float quartInOut(float t)
        {
            if (t < 0.5F)
                return 8.0F * t * t * t * t;
            else
            {
                t -= 1.0F;
                return 1.0F - 8.0F * t * t * t * t;
            }
        }

        static float quintIn(float t)
        {
            return t * t * t * t * t;
        }

        static float quintOut(float t)
        {
            t -= 1.0F;
            return 1.0F + t * t * t * t * t;
        }

        static float quintInOut(float t)
        {
            if (t < 0.5F)
                return 16.0F * t * t * t * t * t;
            else
            {
                t -= 1.0F;
                return 1.0F + 16.0F * t * t * t * t * t;
            }
        }

        static float expoIn(float t)
        {
            return pow(2.0F, 10.0F * (t - 1.0F));
        }

        static float expoOut(float t)
        {
            return 1.0F - pow(2.0F, -10.0F * t);
        }

        static float expoInOut(float t)
        {
            if (t < 0.5F)
                return 0.5F * pow(2.0F, 10.0F * (2.0F * t - 1.0F));
            else
                return 0.5F * (pow(2.0F, -10.0F * (t * 2.0F - 1.0F)) - 2.0F);
        }

        static float circIn(float t)
        {
            return 1.0F - sqrt(1.0F - t * t);
        }

        static float circOut(float t)
        {
            t -= 1.0F;
            return sqrt(1.0F - t * t);
        }

        static float circInOut(float t)
        {
            if (t < 0.5F)
            {
                t *= 2.0F;
                return 0.5F * (-sqrt(1.0F - t * t) + 1.0F);
            }
            else
            {
                t = t * 2.0F - 2.0F;
                return 0.5F * (sqrt(1.0F - t * t) + 1.0F);
            }
        }

        static float backIn(float t)
        {
            static constexpr float s = 1.70158F;
            return t * t * ((s + 1.0F) * t - s);
        }

        static float backOut(float t)
        {
            static constexpr float s = 1.70158F;
            t -= 1.0F;
            return t * t * ((s + 1.0F) * t + s) + 1.0F;
        }

        static float backInOut(float t)
        {
            static constexpr float s = 1.70158F * 1.525F;
            if (t < 0.5F)
            {
                t *= 2.0F;
                return 0.5F * (t * t * ((s + 1.0F) * t - s));
            }
            else
            {
                t = t * 2.0F - 2.0F;
                return 0.5F * (t * t * ((s + 1.0F) * t + s) + 2.0F);
            }
        }

        static float elasticIn(float t)
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            static constexpr float p = 0.3F;

            t -= 1.0F;

            return -pow(2.0F, 10.0F * t) * sin((t - p / 4.0F) * (2.0F * pi<float>) / p);
        }

        static float elasticOut(float t)
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            static constexpr float p = 0.3F;

            return pow(2.0F, -10.0F * t) * sin((t - p / 4.0F) * (2.0F * pi<float>) / p) + 1.0F;
        }

        static float elasticInOut(float t)
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            static constexpr float p = 0.3F * 1.5F;

            if (t < 0.5F)
            {
                t = t * 2.0F - 1.0F;
                return -0.5F * (pow(2.0F, 10.0F * t) * sin((t - p / 4.0F) * (2.0F * pi<float>) / p));
            }
            else
            {
                t = t * 2.0F - 1.0F;
                return 0.5F * pow(2.0F, -10.0F * t) * sin((t - p / 4.0F) * (2.0F * pi<float>) / p) + 1.0F;
            }
        }

        static float bounceOut(float t)
        {
            if (t < (1.0F / 2.75F))
                return 7.5625F * t * t;
            else if (t < (2.0F / 2.75F))
            {
                t -= 1.5F / 2.75F;
                return 7.5625F * t * t + 0.75F;
            }
            else if (t < (2.5/2.75))
            {
                t -= 2.25F / 2.75F;
                return 7.5625F * t * t + 0.9375F;
            }
            else
            {
                t -= 2.625F / 2.75F;
                return 7.5625F * t * t + 0.984375F;
            }
        }

        static float bounceIn(float t)
        {
            return 1.0F - bounceOut(1.0F - t);
        }

        static float bounceInOut(float t)
        {
            if (t < 0.5F)
                return bounceOut(t * 2.0F) * 0.5F;
            else
                return bounceOut(t * 2.0F - 1.0F) * 0.5F + 0.5F;
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
                case Mode::EASE_IN:
                {
                    switch (func)
                    {
                        case Func::SINE: progress = sineIn(progress); break;
                        case Func::QUAD: progress = quadIn(progress); break;
                        case Func::CUBIC: progress = cubicIn(progress); break;
                        case Func::QUART: progress = quartIn(progress); break;
                        case Func::QUINT: progress = quintIn(progress); break;
                        case Func::EXPO: progress = expoIn(progress); break;
                        case Func::CIRC: progress = circIn(progress); break;
                        case Func::BACK: progress = backIn(progress); break;
                        case Func::ELASTIC: progress = elasticIn(progress); break;
                        case Func::BOUNCE: progress = bounceIn(progress); break;
                        default: return;
                    }
                    break;
                }

                case Mode::EASE_OUT:
                {
                    switch (func)
                    {
                        case Func::SINE: progress = sineOut(progress); break;
                        case Func::QUAD: progress = quadOut(progress); break;
                        case Func::CUBIC: progress = cubicOut(progress); break;
                        case Func::QUART: progress = quartOut(progress); break;
                        case Func::QUINT: progress = quintOut(progress); break;
                        case Func::EXPO: progress = expoOut(progress); break;
                        case Func::CIRC: progress = circOut(progress); break;
                        case Func::BACK: progress = backOut(progress); break;
                        case Func::ELASTIC: progress = elasticOut(progress); break;
                        case Func::BOUNCE: progress = bounceOut(progress); break;
                        default: return;
                    }
                    break;
                }

                case Mode::EASE_INOUT:
                {
                    switch (func)
                    {
                        case Func::SINE: progress = sineInOut(progress); break;
                        case Func::QUAD: progress = quadInOut(progress); break;
                        case Func::CUBIC: progress = cubicInOut(progress); break;
                        case Func::QUART: progress = quartInOut(progress); break;
                        case Func::QUINT: progress = quintInOut(progress); break;
                        case Func::EXPO: progress = expoInOut(progress); break;
                        case Func::CIRC: progress = circInOut(progress); break;
                        case Func::BACK: progress = backInOut(progress); break;
                        case Func::ELASTIC: progress = elasticInOut(progress); break;
                        case Func::BOUNCE: progress = bounceInOut(progress); break;
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
                float animationLength = animator->getLength();

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

                    float remainingTime = currentTime - animators.front()->getLength() * static_cast<float>(currentCount);
                    animators.front()->setProgress(remainingTime / animators.front()->getLength());

                    auto resetEvent = std::make_unique<AnimationEvent>();
                    resetEvent->type = Event::Type::ANIMATION_RESET;
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
                    finishEvent->type = Event::Type::ANIMATION_FINISH;
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
                float x = length * progress * timeScale;

                auto x1 = static_cast<uint64_t>(x);
                auto x2 = x1 + 1;
                auto t = x - static_cast<float>(x1);

                Vector3F previousPosition;
                Vector3F nextPosition;

                if (x1 != 0)
                {
                    previousPosition.v[0] = (2.0F * (static_cast<float>(fnvHash(seedX | (x1 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0F) * distance.v[0];
                    previousPosition.v[1] = (2.0F * (static_cast<float>(fnvHash(seedY | (x1 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0F) * distance.v[1];
                    previousPosition.v[2] = (2.0F * (static_cast<float>(fnvHash(seedZ | (x1 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0F) * distance.v[2];
                }

                if (x2 != static_cast<uint32_t>(timeScale))
                {
                    nextPosition.v[0] = (2.0F * (static_cast<float>(fnvHash(seedX | (x2 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0F) * distance.v[0];
                    nextPosition.v[1] = (2.0F * (static_cast<float>(fnvHash(seedY | (x2 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0F) * distance.v[1];
                    nextPosition.v[2] = (2.0F * (static_cast<float>(fnvHash(seedZ | (x2 << 32))) / std::numeric_limits<uint64_t>::max()) - 1.0F) * distance.v[2];
                }

                Vector3F noise(smoothStep(previousPosition.v[0], nextPosition.v[0], t),
                                     smoothStep(previousPosition.v[1], nextPosition.v[1], t),
                                     smoothStep(previousPosition.v[2], nextPosition.v[2], t));

                targetActor->setPosition(startPosition + noise);
            }
        }
    } // namespace scene
} // namespace ouzel
