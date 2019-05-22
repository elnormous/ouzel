// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_ANIMATORS_HPP
#define OUZEL_SCENE_ANIMATORS_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include "scene/Animator.hpp"
#include "scene/Component.hpp"
#include "events/EventHandler.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    namespace scene
    {
        class Ease final: public Animator
        {
        public:
            enum class Mode
            {
                EASE_IN,
                EASE_OUT,
                EASE_INOUT
            };

            enum class Func
            {
                SINE,
                QUAD,
                CUBIC,
                QUART,
                QUINT,
                EXPO,
                CIRC,
                BACK,
                ELASTIC,
                BOUNCE
            };

            Ease(Animator* animator, Mode initModee, Func initFunc);
            Ease(const std::unique_ptr<Animator>& animator, Mode initMode, Func initFunc);

        protected:
            void updateProgress() final;

        private:
            Mode mode;
            Func func;
        };

        class Fade final: public Animator
        {
        public:
            Fade(float initLength, float initOpacity, bool initRelative = false);

            void play() final;

        protected:
            void updateProgress() final;

        private:
            float opacity;
            float startOpacity = 0.0F;
            float targetOpacity = 0.0F;
            float diff = 0.0F;
            bool relative;
        };

        class Move final: public Animator
        {
        public:
            Move(float initLength, const Vector3F& initPosition, bool initRelative = false);

            void play() final;

        protected:
            void updateProgress() final;

        private:
            Vector3F position;
            Vector3F startPosition;
            Vector3F targetPosition;
            Vector3F diff;
            bool relative;
        };

        class Parallel final: public Animator
        {
        public:
            explicit Parallel(const std::vector<Animator*>& initAnimators);
            explicit Parallel(const std::vector<std::unique_ptr<Animator>>& initAnimators);

        protected:
            void updateProgress() final;
        };

        class Repeat final: public Animator
        {
        public:
            explicit Repeat(Animator* animator, uint32_t initCount = 0);
            explicit Repeat(const std::unique_ptr<Animator>& animator, uint32_t initCount = 0);

            void reset() final;

        protected:
            void updateProgress() final;

        private:
            uint32_t count = 0;
            uint32_t currentCount = 0;
        };

        class Rotate final: public Animator
        {
        public:
            Rotate(float initLength, const Vector3F& initRotation, bool initRelative = false);

            void play() final;

        protected:
            void updateProgress() final;

        private:
            Vector3F rotation;
            Vector3F startRotation;
            Vector3F targetRotation;
            Vector3F diff;
            bool relative;
        };

        class Scale final: public Animator
        {
        public:
            Scale(float initLength, const Vector3F& initScale, bool initRelative = false);

            void play() final;

        protected:
            void updateProgress() final;

        private:
            Vector3F scale;
            Vector3F startScale;
            Vector3F targetScale;
            Vector3F diff;
            bool relative;
        };

        class Sequence final: public Animator
        {
        public:
            explicit Sequence(const std::vector<Animator*>& initAnimators);
            explicit Sequence(const std::vector<std::unique_ptr<Animator>>& initAnimators);

            void play() final;

        protected:
            void updateProgress() final;

        private:
            Animator* currentAnimator = nullptr;
        };

        class Shake final: public Animator
        {
        public:
            Shake(float initLength, const Vector3F& initDistance, float initTimeScale);

            void play() final;

        protected:
            void updateProgress() final;

        private:
            uint32_t seedX;
            uint32_t seedY;
            uint32_t seedZ;
            Vector3F distance;
            float timeScale;
            Vector3F startPosition;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_ANIMATORS_HPP
