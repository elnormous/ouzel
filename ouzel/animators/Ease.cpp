// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cmath>
#include "Ease.h"
#include "math/MathUtils.h"

namespace ouzel
{
    namespace scene
    {
        static float sineIn(float t)
        {
            return 1.0f - cosf(t * PI_2);
        }

        static float sineOut(float t)
        {
            return sinf(t * PI_2);
        }

        static float sineInOut(float t)
        {
            return -0.5f * (cosf(PI * t) - 1.0f);
        }

        static float quadIn(float t)
        {
            return t * t;
        }

        static float quadOut(float t)
        {
            return t * (2.0f - t);
        }

        static float quadInOut(float t)
        {
            if (t < 0.5f)
                return 2.0f * t * t;
            else
                return (-1.0f + (4.0f - 2.0f * t) * t);
        }

        static float cubicIn(float t)
        {
            return t * t * t;
        }

        static float cubicOut(float t)
        {
            t -= 1.0f;
            return t * t * t + 1.0f;
        }

        static float cubicInOut(float t)
        {
            if (t < 0.5f)
                return 4.0f * t * t * t;
            else
                return (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
        }

        static float quartIn(float t)
        {
            return t * t * t * t;
        }

        static float quartOut(float t)
        {
            t -= 1.0f;
            return 1.0f - t * t * t;
        }

        static float quartInOut(float t)
        {
            if (t < 0.5f)
                return 8.0f * t * t * t * t;
            else
            {
                t -= 1.0f;
                return 1.0f - 8.0f * t * t * t * t;
            }
        }

        static float quintIn(float t)
        {
            return t * t * t * t * t;
        }

        static float quintOut(float t)
        {
            t -= 1.0f;
            return 1.0f + t * t * t * t * t;
        }

        static float quintInOut(float t)
        {
            if (t < 0.5f)
                return 16.0f * t * t * t * t * t;
            else
            {
                t -= 1.0f;
                return 1.0f + 16.0f * t * t * t * t * t;
            }
        }

        static float expoIn(float t)
        {
            return powf(2.0f, 10.0f * (t - 1.0f));
        }

        static float expoOut(float t)
        {
            return 1.0f - powf(2.0f, -10.0f * t);
        }

        static float expoInOut(float t)
        {
            if (t < 0.5f)
                return 0.5f * powf(2.0f, 10.0f * (2.0f * t - 1.0f));
            else
                return 0.5f * (powf(2.0f, -10.0f * (t * 2.0f - 1.0f)) - 2.0f);
        }

        static float circIn(float t)
        {
            return 1.0f - sqrtf(1.0f - t * t);
        }

        static float circOut(float t)
        {
            t -= 1.0f;
            return sqrtf(1.0f - t * t);
        }

        static float circInOut(float t)
        {
            if (t < 0.5f)
            {
                t *= 2.0f;
                return 0.5f * (-sqrtf(1.0f - t * t) + 1.0f);
            }
            else
            {
                t = t * 2.0f - 2.0f;
                return 0.5f * (sqrtf(1.0f - t * t) + 1.0f);
            }
        }

        static float backIn(float t)
        {
            const float s = 1.70158f;
            return t * t * ((s + 1.0f) * t - s);
        }

        static float backOut(float t)
        {
            const float s = 1.70158f;
            t -= 1.0f;
            return t * t * ((s + 1.0f) * t + s) + 1.0f;
        }

        static float backInOut(float t)
        {
            const float s = 1.70158f * 1.525f;
            if (t < 0.5f)
            {
                t *= 2.0f;
                return 0.5f * (t * t * ((s + 1.0f) * t - s));
            }
            else
            {
                t = t * 2.0f - 2.0f;
                return 0.5f * (t * t * ((s + 1.0f) * t + s) + 2.0f);
            }
        }

        static float elasticIn(float t)
        {
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;

            float p = 0.3f;

            t -= 1.0f;

            return -powf(2.0f, 10.0f * t) * sinf((t - p / 4.0f) * (2.0f * PI) / p);
        }

        static float elasticOut(float t)
        {
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;

            float p = 0.3f;

            return powf(2.0f, -10.0f * t) * sinf((t - p / 4.0f) * (2.0f * PI) / p) + 1.0f;
        }

        static float elasticInOut(float t)
        {
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;

            float p = 0.3f * 1.5f;

            if (t < 0.5f)
            {
                t = t * 2.0f - 1.0f;
                return -0.5f * (powf(2.0f, 10.0f * t) * sinf((t - p / 4.0f) * (2.0f * PI) / p));
            }
            else
            {
                t = t * 2.0f - 1.0f;
                return 0.5f * powf(2.0f, -10.0f * t) * sinf((t - p / 4.0f) * (2.0f * PI) / p) + 1.0f;
            }
        }

        static float bounceOut(float t)
        {
            if (t < (1.0f / 2.75f))
            {
                return 7.5625f * t * t;
            }
            else if (t < (2.0f / 2.75f))
            {
                t -= 1.5f / 2.75f;
                return 7.5625f * t * t + 0.75f;
            }
            else if (t < (2.5/2.75))
            {
                t -= 2.25f / 2.75f;
                return 7.5625f * t * t + 0.9375f;
            }
            else
            {
                t -= 2.625f / 2.75f;
                return 7.5625f * t * t + 0.984375f;
            }
        }

        static float bounceIn(float t)
        {
            return 1.0f - bounceOut(1.0f - t);
        }

        static float bounceInOut(float t)
        {
            if (t < 0.5f)
                return bounceOut(t * 2.0f) * 0.5f;
            else
                return bounceOut(t * 2.0f - 1.0f) * 0.5f + 0.5f;
        }

        Ease::Ease(const AnimatorPtr& aAnimator, Type aType, Func aFunc):
            Animator(aAnimator->getLength()), animator(aAnimator), type(aType), func(aFunc)
        {
        }

        void Ease::start(Node* targetNode)
        {
            Animator::start(targetNode);

            animator->start(targetNode);
        }

        void Ease::reset()
        {
            Animator::reset();

            animator->reset();
        }

        void Ease::updateProgress()
        {
            Animator::updateProgress();

            switch (type)
            {
                case Type::IN:
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
                    }
                    break;
                }

                case Type::OUT:
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
                    }
                    break;
                }

                case Type::INOUT:
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
                    }
                    break;
                }
            }

            animator->setProgress(progress);
        }
    } // namespace scene
} // namespace ouzel
