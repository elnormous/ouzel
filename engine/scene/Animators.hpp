// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_ANIMATORS_HPP
#define OUZEL_SCENE_ANIMATORS_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include "Animator.hpp"
#include "Component.hpp"
#include "../events/EventHandler.hpp"
#include "../math/Quaternion.hpp"
#include "../math/Vector.hpp"

namespace ouzel::scene
{
    class Ease final: public Animator
    {
    public:
        enum class Mode
        {
            easeIn,
            easeOut,
            easeInOut
        };

        enum class Func
        {
            sine,
            quad,
            cubic,
            quart,
            quint,
            expo,
            circ,
            back,
            elastic,
            bounce
        };

        Ease(Animator& animator, Mode initModee, Func initFunc);

    private:
        void updateProgress() final;

        Mode mode;
        Func func;
    };

    class Fade final: public Animator
    {
    public:
        Fade(float initLength, float initOpacity, bool initRelative = false);

        void play() final;

    private:
        void updateProgress() final;

        float opacity;
        float startOpacity = 0.0F;
        float targetOpacity = 0.0F;
        float diff = 0.0F;
        bool relative;
    };

    class Move final: public Animator
    {
    public:
        Move(float initLength, const Vector<float, 3>& initPosition, bool initRelative = false);

        void play() final;

    private:
        void updateProgress() final;

        Vector<float, 3> position;
        Vector<float, 3> startPosition;
        Vector<float, 3> targetPosition;
        Vector<float, 3> diff;
        bool relative;
    };

    class Parallel final: public Animator
    {
    public:
        explicit Parallel(const std::vector<Animator*>& initAnimators);
        explicit Parallel(const std::vector<std::unique_ptr<Animator>>& initAnimators);

    private:
        void updateProgress() final;
    };

    class Repeat final: public Animator
    {
    public:
        explicit Repeat(Animator& animator, std::uint32_t initCount = 0);

        void reset() final;

    private:
        void updateProgress() final;

        std::uint32_t count = 0;
        std::uint32_t currentCount = 0;
    };

    class Rotate final: public Animator
    {
    public:
        Rotate(float initLength, const Vector<float, 3>& initRotation, bool initRelative = false);

        void play() final;

    private:
        void updateProgress() final;

        Vector<float, 3> rotation;
        Vector<float, 3> startRotation;
        Vector<float, 3> targetRotation;
        Vector<float, 3> diff;
        bool relative;
    };

    class Scale final: public Animator
    {
    public:
        Scale(float initLength, const Vector<float, 3>& initScale, bool initRelative = false);

        void play() final;

    private:
        void updateProgress() final;

        Vector<float, 3> scale;
        Vector<float, 3> startScale;
        Vector<float, 3> targetScale;
        Vector<float, 3> diff;
        bool relative;
    };

    class Sequence final: public Animator
    {
    public:
        explicit Sequence(const std::vector<Animator*>& initAnimators);
        explicit Sequence(const std::vector<std::unique_ptr<Animator>>& initAnimators);

        void play() final;

    private:
        void updateProgress() final;

        Animator* currentAnimator = nullptr;
    };

    class Shake final: public Animator
    {
    public:
        Shake(float initLength, const Vector<float, 3>& initDistance, float initTimeScale);

        void play() final;

    private:
        void updateProgress() final;

        std::uint32_t seedX;
        std::uint32_t seedY;
        std::uint32_t seedZ;
        Vector<float, 3> distance;
        float timeScale;
        Vector<float, 3> startPosition;
    };
}

#endif // OUZEL_SCENE_ANIMATORS_HPP
