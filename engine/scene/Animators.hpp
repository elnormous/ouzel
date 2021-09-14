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
        void updateProgress() override;

        Mode mode;
        Func func;
    };

    class Fade final: public Animator
    {
    public:
        Fade(float initLength, float initOpacity, bool initRelative = false);

        void play() override;

    private:
        void updateProgress() override;

        float opacity = 1.0F;
        float startOpacity = 0.0F;
        float targetOpacity = 0.0F;
        float diff = 0.0F;
        bool relative = false;
    };

    class Move final: public Animator
    {
    public:
        Move(float initLength, const math::Vector<float, 3>& initPosition, bool initRelative = false);

        void play() override;

    private:
        void updateProgress() override;

        math::Vector<float, 3> position{};
        math::Vector<float, 3> startPosition{};
        math::Vector<float, 3> targetPosition{};
        math::Vector<float, 3> diff{};
        bool relative = false;
    };

    class Parallel final: public Animator
    {
    public:
        explicit Parallel(const std::vector<Animator*>& initAnimators);
        explicit Parallel(const std::vector<std::unique_ptr<Animator>>& initAnimators);

    private:
        void updateProgress() override;
    };

    class Repeat final: public Animator
    {
    public:
        explicit Repeat(Animator& animator, std::uint32_t initCount = 0);

        void reset() override;

    private:
        void updateProgress() override;

        std::uint32_t count = 0;
        std::uint32_t currentCount = 0;
    };

    class Rotate final: public Animator
    {
    public:
        Rotate(float initLength, const math::Vector<float, 3>& initRotation, bool initRelative = false);

        void play() override;

    private:
        void updateProgress() override;

        math::Vector<float, 3> rotation{};
        math::Vector<float, 3> startRotation{};
        math::Vector<float, 3> targetRotation{};
        math::Vector<float, 3> diff{};
        bool relative = false;
    };

    class Scale final: public Animator
    {
    public:
        Scale(float initLength, const math::Vector<float, 3>& initScale, bool initRelative = false);

        void play() override;

    private:
        void updateProgress() override;

        math::Vector<float, 3> scale{};
        math::Vector<float, 3> startScale{};
        math::Vector<float, 3> targetScale{};
        math::Vector<float, 3> diff{};
        bool relative = false;
    };

    class Sequence final: public Animator
    {
    public:
        explicit Sequence(const std::vector<Animator*>& initAnimators);
        explicit Sequence(const std::vector<std::unique_ptr<Animator>>& initAnimators);

        void play() override;

    private:
        void updateProgress() override;

        Animator* currentAnimator = nullptr;
    };

    class Shake final: public Animator
    {
    public:
        Shake(float initLength, const math::Vector<float, 3>& initDistance, float initTimeScale);

        void play() override;

    private:
        void updateProgress() override;

        std::uint32_t seedX;
        std::uint32_t seedY;
        std::uint32_t seedZ;
        math::Vector<float, 3> distance{};
        float timeScale = 1.0F;
        math::Vector<float, 3> startPosition{};
    };
}

#endif // OUZEL_SCENE_ANIMATORS_HPP
