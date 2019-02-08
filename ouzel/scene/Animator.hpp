// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ANIMATOR_HPP
#define OUZEL_ANIMATOR_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include "scene/Component.hpp"
#include "events/EventHandler.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace scene
    {
        class Animator: public Component
        {
            friend Actor;
        public:
            explicit Animator(float initLength);
            virtual ~Animator();

            virtual void update(float delta);

            virtual void start();
            virtual void play();

            virtual void resume();
            virtual void stop(bool resetAnimation = false);
            virtual void reset();

            bool isRunning() const { return running; }
            bool isDone() const { return done; }

            inline float getLength() const { return length; }
            inline float getCurrentTime() const { return currentTime; }

            inline float getProgress() const { return progress; }
            virtual void setProgress(float newProgress);

            inline Actor* getTargetActor() const { return targetActor; }

            void addAnimator(std::unique_ptr<Animator>&& animator);
            void addAnimator(Animator* animator);
            bool removeAnimator(Animator* animator);
            void removeAllAnimators();

            inline Animator* getParent() const { return parent; }
            void removeFromParent();

        protected:
            bool handleUpdate(const UpdateEvent& event);
            virtual void updateProgress() {}

            float length = 0.0F;
            float currentTime = 0.0F;
            float progress = 0.0F;
            bool done = false;
            bool running = false;

            Animator* parent = nullptr;
            Actor* targetActor = nullptr;

            EventHandler updateHandler;

            std::vector<Animator*> animators;
            std::vector<std::unique_ptr<Animator>> ownedAnimators;
        };

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
            void updateProgress() override;

        private:
            Mode mode;
            Func func;
        };

        class Fade final: public Animator
        {
        public:
            Fade(float initLength, float initOpacity, bool initRelative = false);

            void play() override;

        protected:
            void updateProgress() override;

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
            Move(float initLength, const Vector3<float>& initPosition, bool initRelative = false);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            Vector3<float> position;
            Vector3<float> startPosition;
            Vector3<float> targetPosition;
            Vector3<float> diff;
            bool relative;
        };

        class Parallel final: public Animator
        {
        public:
            explicit Parallel(const std::vector<Animator*>& initAnimators);
            explicit Parallel(const std::vector<std::unique_ptr<Animator>>& initAnimators);

        protected:
            void updateProgress() override;
        };

        class Repeat final: public Animator
        {
        public:
            explicit Repeat(Animator* animator, uint32_t initCount = 0);
            explicit Repeat(const std::unique_ptr<Animator>& animator, uint32_t initCount = 0);

            void reset() override;

        protected:
            void updateProgress() override;

        private:
            uint32_t count = 0;
            uint32_t currentCount = 0;
        };

        class Rotate final: public Animator
        {
        public:
            Rotate(float initLength, const Vector3<float>& initRotation, bool initRelative = false);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            Vector3<float> rotation;
            Vector3<float> startRotation;
            Vector3<float> targetRotation;
            Vector3<float> diff;
            bool relative;
        };

        class Scale final: public Animator
        {
        public:
            Scale(float initLength, const Vector3<float>& initScale, bool initRelative = false);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            Vector3<float> scale;
            Vector3<float> startScale;
            Vector3<float> targetScale;
            Vector3<float> diff;
            bool relative;
        };

        class Sequence final: public Animator
        {
        public:
            explicit Sequence(const std::vector<Animator*>& initAnimators);
            explicit Sequence(const std::vector<std::unique_ptr<Animator>>& initAnimators);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            Animator* currentAnimator = nullptr;
        };

        class Shake final: public Animator
        {
        public:
            Shake(float initLength, const Vector3<float>& initDistance, float initTimeScale);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            uint32_t seedX;
            uint32_t seedY;
            uint32_t seedZ;
            Vector3<float> distance;
            float timeScale;
            Vector3<float> startPosition;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_ANIMATOR_HPP
