// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cmath>
#include "Ease.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace scene
    {
        static float sineIn(float t)
        {
            return 1.0F - cosf(t * PI / 2.0F);
        }

        static float sineOut(float t)
        {
            return sinf(t * PI / 2.0F);
        }

        static float sineInOut(float t)
        {
            return -0.5F * (cosf(PI * t) - 1.0F);
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
            return powf(2.0F, 10.0F * (t - 1.0F));
        }

        static float expoOut(float t)
        {
            return 1.0F - powf(2.0F, -10.0F * t);
        }

        static float expoInOut(float t)
        {
            if (t < 0.5F)
                return 0.5F * powf(2.0F, 10.0F * (2.0F * t - 1.0F));
            else
                return 0.5F * (powf(2.0F, -10.0F * (t * 2.0F - 1.0F)) - 2.0F);
        }

        static float circIn(float t)
        {
            return 1.0F - sqrtf(1.0F - t * t);
        }

        static float circOut(float t)
        {
            t -= 1.0F;
            return sqrtf(1.0F - t * t);
        }

        static float circInOut(float t)
        {
            if (t < 0.5F)
            {
                t *= 2.0F;
                return 0.5F * (-sqrtf(1.0F - t * t) + 1.0F);
            }
            else
            {
                t = t * 2.0F - 2.0F;
                return 0.5F * (sqrtf(1.0F - t * t) + 1.0F);
            }
        }

        static float backIn(float t)
        {
            const float s = 1.70158F;
            return t * t * ((s + 1.0F) * t - s);
        }

        static float backOut(float t)
        {
            const float s = 1.70158F;
            t -= 1.0F;
            return t * t * ((s + 1.0F) * t + s) + 1.0F;
        }

        static float backInOut(float t)
        {
            const float s = 1.70158F * 1.525F;
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

            float p = 0.3F;

            t -= 1.0F;

            return -powf(2.0F, 10.0F * t) * sinf((t - p / 4.0F) * (2.0F * PI) / p);
        }

        static float elasticOut(float t)
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            float p = 0.3F;

            return powf(2.0F, -10.0F * t) * sinf((t - p / 4.0F) * (2.0F * PI) / p) + 1.0F;
        }

        static float elasticInOut(float t)
        {
            if (t == 0.0F) return 0.0F;
            if (t == 1.0F) return 1.0F;

            float p = 0.3F * 1.5F;

            if (t < 0.5F)
            {
                t = t * 2.0F - 1.0F;
                return -0.5F * (powf(2.0F, 10.0F * t) * sinf((t - p / 4.0F) * (2.0F * PI) / p));
            }
            else
            {
                t = t * 2.0F - 1.0F;
                return 0.5F * powf(2.0F, -10.0F * t) * sinf((t - p / 4.0F) * (2.0F * PI) / p) + 1.0F;
            }
        }

        static float bounceOut(float t)
        {
            if (t < (1.0F / 2.75F))
            {
                return 7.5625F * t * t;
            }
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

        Ease::Ease(Animator* animator, Mode initMode, Func initFunc):
            Animator(animator->getLength()), mode(initMode), func(initFunc)
        {
            addAnimator(animator);
        }

        Ease::Ease(const std::unique_ptr<Animator>& animator, Mode initMode, Func initFunc):
            Ease(animator.get(), initMode, initFunc)
        {
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
                        case Func::QUINT: quintOut(progress); break;
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
    } // namespace scene
} // namespace ouzel
