#ifndef FLUXION_SCENE_ANIMATORS_HPP
#define FLUXION_SCENE_ANIMATORS_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include "Animator.hpp"
#include "Component.hpp"
#include "../easing/Easing.hpp"
#include "../events/EventHandler.hpp"
#include "../math/Quaternion.hpp"
#include "../math/Vector.hpp"
#include "../reflection/Macros.hpp"

namespace fluxion::scene
{
    class Ease final: public Animator
    {
    public:
        Ease(Animator& animator, easing::Func initFunc, easing::Mode initModee);

    private:
        void updateProgress() override;

        easing::Func func;
        easing::Mode mode;
    };

    class Fade final: public Animator
    {
    public:
        FLUXION_COMPONENT_DECL(Fade, "scene.Fade")

        // Default-construct a 1-second fade-to-fully-opaque so the
        // editor's TypeRegistry can spawn a usable instance from
        // "Add Component". Inspector exposes targetOpacity and
        // duration so the user can dial it in.
        Fade();
        Fade(float initLength, float initOpacity, bool initRelative = false);

        void play() override;

        auto getTargetOpacity() const noexcept { return opacity; }
        void setTargetOpacity(float newOpacity) noexcept { opacity = newOpacity; }
        auto isRelative() const noexcept { return relative; }
        void setRelative(bool newRelative) noexcept { relative = newRelative; }

        void inspect(reflection::Inspector& inspector) override;

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
        FLUXION_COMPONENT_DECL(Move, "scene.Move")

        Move();
        Move(float initLength, const math::Vector<float, 3>& initPosition, bool initRelative = false);

        void play() override;

        auto& getTargetPosition() const noexcept { return position; }
        void setTargetPosition(const math::Vector<float, 3>& newPosition) noexcept { position = newPosition; }
        auto isRelative() const noexcept { return relative; }
        void setRelative(bool newRelative) noexcept { relative = newRelative; }

        void inspect(reflection::Inspector& inspector) override;

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
        FLUXION_COMPONENT_DECL(Rotate, "scene.Rotate")

        Rotate();
        Rotate(float initLength, const math::Vector<float, 3>& initRotation, bool initRelative = false);

        void play() override;

        auto& getTargetRotation() const noexcept { return rotation; }
        void setTargetRotation(const math::Vector<float, 3>& newRotation) noexcept { rotation = newRotation; }
        auto isRelative() const noexcept { return relative; }
        void setRelative(bool newRelative) noexcept { relative = newRelative; }

        void inspect(reflection::Inspector& inspector) override;

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
        FLUXION_COMPONENT_DECL(Scale, "scene.Scale")

        Scale();
        Scale(float initLength, const math::Vector<float, 3>& initScale, bool initRelative = false);

        void play() override;

        auto& getTargetScale() const noexcept { return scale; }
        void setTargetScale(const math::Vector<float, 3>& newScale) noexcept { scale = newScale; }
        auto isRelative() const noexcept { return relative; }
        void setRelative(bool newRelative) noexcept { relative = newRelative; }

        void inspect(reflection::Inspector& inspector) override;

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

#endif // FLUXION_SCENE_ANIMATORS_HPP
