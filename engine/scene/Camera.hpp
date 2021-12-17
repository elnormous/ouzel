// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_CAMERA_HPP
#define OUZEL_SCENE_CAMERA_HPP

#include <memory>
#include "Component.hpp"
#include "../math/Constants.hpp"
#include "../math/Matrix.hpp"
#include "../math/Rect.hpp"
#include "../graphics/DepthStencilState.hpp"
#include "../graphics/RenderTarget.hpp"

namespace ouzel::scene
{
    class Layer;

    class Camera: public Component
    {
        friend Layer;
    public:
        enum class ProjectionMode
        {
            custom,
            orthographic,
            perspective
        };

        enum class ScaleMode
        {
            noScale,
            exactFit,
            noBorder,
            showAll
        };

        explicit Camera(const math::Matrix<float, 4>& initProjection);
        explicit Camera(const math::Size<float, 2>& initTargetContentSize = math::Size<float, 2>{}, ScaleMode initScaleMode = ScaleMode::noScale);
        explicit Camera(float initFov, float initNearPlane = 1.0F, float initFarPlane = 100.0F);
        ~Camera() override;

        [[nodiscard]] auto getProjectionMode() const noexcept { return projectionMode; }
        void setProjectionMode(ProjectionMode newProjectionMode)
        {
            projectionMode = newProjectionMode;
            projectionDirty = true;
        }

        [[nodiscard]] auto getFov() const noexcept { return fov; }
        void setFov(float newFov)
        {
            fov = newFov;
            projectionDirty = viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        [[nodiscard]] auto getNearPlane() const noexcept { return nearPlane; }
        void setNearPlane(float newNearPlane)
        {
            nearPlane = newNearPlane;
            projectionDirty = viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        [[nodiscard]] auto getFarPlane() const noexcept { return farPlane; }
        void setFarPlane(float newFarPlane)
        {
            farPlane = newFarPlane;
            projectionDirty = viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        [[nodiscard]] auto& getProjection() const noexcept { return projection; }
        void setProjection(const math::Matrix<float, 4>& newProjection)
        {
            projection = newProjection;
            projectionDirty = viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        const math::Matrix<float, 4>& getViewProjection() const;
        const math::Matrix<float, 4>& getRenderViewProjection() const;
        const math::Matrix<float, 4>& getInverseViewProjection() const;

        math::Vector<float, 3> convertClipToWorld(const math::Vector<float, 3>& clipPosition) const noexcept;
        math::Vector<float, 3> convertWorldToClip(const math::Vector<float, 3>& worldPosition) const noexcept;

        math::Vector<float, 3> convertNormalizedToWorld(const math::Vector<float, 2>& normalizedPosition) const noexcept;
        math::Vector<float, 2> convertWorldToNormalized(const math::Vector<float, 3>& worldPosition) const noexcept;

        bool checkVisibility(const math::Matrix<float, 4>& boxTransform, const math::Box<float, 3>& box) const;

        [[nodiscard]] auto& getViewport() const noexcept { return viewport; }
        void setViewport(const math::Rect<float>& newViewport);
        [[nodiscard]] auto& getRenderViewport() const noexcept { return renderViewport; }

        [[nodiscard]] auto getScaleMode() const noexcept { return scaleMode; }
        void setScaleMode(ScaleMode newScaleMode);

        [[nodiscard]] auto& getTargetContentSize() const noexcept { return targetContentSize; }
        void setTargetContentSize(const math::Size<float, 2>& newTargetContentSize);

        [[nodiscard]] auto& getContentSize() const noexcept { return contentSize; }
        [[nodiscard]] auto& getContentScale() const noexcept { return contentScale; }
        [[nodiscard]] auto& getContentPosition() const noexcept { return contentPosition; }

        [[nodiscard]] auto getRenderTarget() const noexcept { return renderTarget; }
        void setRenderTarget(graphics::RenderTarget* newRenderTarget);

        [[nodiscard]] auto getDepthTest() const noexcept { return depthTest; }
        void setDepthTest(bool newDepthTest);
        [[nodiscard]] auto& getDepthStencilState() const noexcept { return depthStencilState; }

        [[nodiscard]] auto getStencilReferenceValue() const noexcept { return stencilReferenceValue; }
        void setStencilReferenceValue(std::uint32_t newStencilReferenceValue) { stencilReferenceValue = newStencilReferenceValue; }

        [[nodiscard]] auto getWireframe() const noexcept { return wireframe; }
        void setWireframe(bool newWireframe) { wireframe = newWireframe; }

        [[nodiscard]] auto getClearColorBuffer() const noexcept { return clearColorBuffer; }
        void setClearColorBuffer(bool clear) { clearColorBuffer = clear; }

        [[nodiscard]] auto getClearDepthBuffer() const noexcept { return clearDepthBuffer; }
        void setClearDepthBuffer(bool clear) { clearDepthBuffer = clear; }

        [[nodiscard]] auto getClearStencilBuffer() const noexcept { return clearStencilBuffer; }
        void setClearStencilBuffer(bool clear) { clearStencilBuffer = clear; }

        [[nodiscard]] auto getClearColor() const noexcept { return clearColor; }
        void setClearColor(math::Color color) { clearColor = color; }

        [[nodiscard]] auto getClearDepth() const noexcept { return clearDepth; }
        void setClearDepth(float depth) { clearDepth = depth; }

        [[nodiscard]] auto getClearStencil() const noexcept { return clearStencil; }
        void setClearDepth(std::uint32_t stencil) { clearStencil = stencil; }

    private:
        void setActor(Actor* newActor) override;
        void setLayer(Layer* newLayer) override;

        void updateTransform() override;
        void calculateProjection() const;
        void calculateViewProjection() const;

        ProjectionMode projectionMode;
        float fov = math::tau<float> / 6.0F;
        float nearPlane = 1.0F;
        float farPlane = 100.0F;

        mutable bool projectionDirty = true;
        mutable math::Matrix<float, 4> projection;

        math::Rect<float> viewport = math::Rect<float>{0.0F, 0.0F, 1.0F, 1.0F};
        mutable math::Rect<float> renderViewport{};
        math::Size<float, 2> targetContentSize{};

        ScaleMode scaleMode = ScaleMode::noScale;
        mutable math::Size<float, 2> contentSize{};
        mutable math::Vector<float, 2> contentScale{};
        mutable math::Vector<float, 2> contentPosition{};

        bool depthTest = false;
        bool wireframe = false;

        mutable bool viewProjectionDirty = true;
        mutable math::Matrix<float, 4> viewProjection;
        mutable math::Matrix<float, 4> renderViewProjection;

        mutable bool inverseViewProjectionDirty = true;
        mutable math::Matrix<float, 4> inverseViewProjection;

        graphics::RenderTarget* renderTarget = nullptr;
        std::unique_ptr<graphics::DepthStencilState> depthStencilState;
        std::uint32_t stencilReferenceValue = 0;

        bool clearColorBuffer = false;
        bool clearDepthBuffer = false;
        bool clearStencilBuffer = false;
        math::Color clearColor;
        float clearDepth = 1.0F;
        std::uint32_t clearStencil = 0;
    };
}

#endif // OUZEL_SCENE_CAMERA_HPP
