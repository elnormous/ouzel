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

        explicit Camera(const Matrix<float, 4>& initProjection);
        explicit Camera(const Size<float, 2>& initTargetContentSize = Size<float, 2>{}, ScaleMode initScaleMode = ScaleMode::noScale);
        explicit Camera(float initFov, float initNearPlane = 1.0F, float initFarPlane = 100.0F);
        ~Camera() override;

        auto getProjectionMode() const noexcept { return projectionMode; }
        void setProjectionMode(ProjectionMode newProjectionMode) { projectionMode = newProjectionMode; }

        auto getFOV() const noexcept { return fov; }
        void setFOV(float newFOV) { fov = newFOV; }

        auto getNearPlane() const noexcept { return nearPlane; }
        void setNearPlane(float newNearPlane) { nearPlane = newNearPlane; }

        auto getFarPlane() const noexcept { return farPlane; }
        void setFarPlane(float newFarPlane) { farPlane = newFarPlane; }

        auto& getProjection() const noexcept { return projection; }
        void recalculateProjection();

        const Matrix<float, 4>& getViewProjection() const;
        const Matrix<float, 4>& getRenderViewProjection() const;
        const Matrix<float, 4>& getInverseViewProjection() const;

        Vector<float, 3> convertNormalizedToWorld(const Vector<float, 2>& normalizedPosition) const;
        Vector<float, 2> convertWorldToNormalized(const Vector<float, 3>& worldPosition) const;

        bool checkVisibility(const Matrix<float, 4>& boxTransform, const Box<float, 3>& box) const;

        auto& getViewport() const noexcept { return viewport; }
        auto& getRenderViewport() const noexcept { return renderViewport; }
        void setViewport(const Rect<float>& newViewport);

        auto getScaleMode() const noexcept { return scaleMode; }
        void setScaleMode(ScaleMode newScaleMode);

        auto& getTargetContentSize() const noexcept { return targetContentSize; }
        void setTargetContentSize(const Size<float, 2>& newTargetContentSize);

        auto& getContentSize() const noexcept { return contentSize; }
        auto& getContentScale() const noexcept { return contentScale; }
        auto& getContentPosition() const noexcept { return contentPosition; }

        auto getRenderTarget() const noexcept { return renderTarget; }
        void setRenderTarget(graphics::RenderTarget* newRenderTarget);

        auto getDepthTest() const noexcept { return depthTest; }
        void setDepthTest(bool newDepthTest);
        auto& getDepthStencilState() const noexcept { return depthStencilState; }

        auto getStencilReferenceValue() const noexcept { return stencilReferenceValue; }
        void setStencilReferenceValue(std::uint32_t newStencilReferenceValue) { stencilReferenceValue = newStencilReferenceValue; }

        auto getWireframe() const noexcept { return wireframe; }
        void setWireframe(bool newWireframe) { wireframe = newWireframe; }

        auto getClearColorBuffer() const noexcept { return clearColorBuffer; }
        void setClearColorBuffer(bool clear) { clearColorBuffer = clear; }

        auto getClearDepthBuffer() const noexcept { return clearDepthBuffer; }
        void setClearDepthBuffer(bool clear) { clearDepthBuffer = clear; }

        auto getClearStencilBuffer() const noexcept { return clearStencilBuffer; }
        void setClearStencilBuffer(bool clear) { clearStencilBuffer = clear; }

        auto getClearColor() const noexcept { return clearColor; }
        void setClearColor(Color color) { clearColor = color; }

        auto getClearDepth() const noexcept { return clearDepth; }
        void setClearDepth(float depth) { clearDepth = depth; }

        auto getClearStencil() const noexcept { return clearStencil; }
        void setClearDepth(std::uint32_t stencil) { clearStencil = stencil; }

    private:
        void setActor(Actor* newActor) override;
        void setLayer(Layer* newLayer) override;

        void updateTransform() override;
        void calculateViewProjection() const;

        ProjectionMode projectionMode;
        float fov = tau<float> / 6.0F;
        float nearPlane = 1.0F;
        float farPlane = 100.0F;

        Matrix<float, 4> projection;

        Rect<float> viewport = Rect<float>{0.0F, 0.0F, 1.0F, 1.0F};
        Rect<float> renderViewport;
        Size<float, 2> targetContentSize;

        ScaleMode scaleMode = ScaleMode::noScale;
        Size<float, 2> contentSize{};
        Vector<float, 2> contentScale{};
        Vector<float, 2> contentPosition{};

        bool depthTest = false;
        bool wireframe = false;

        mutable bool viewProjectionDirty = true;
        mutable Matrix<float, 4> viewProjection;
        mutable Matrix<float, 4> renderViewProjection;

        mutable bool inverseViewProjectionDirty = true;
        mutable Matrix<float, 4> inverseViewProjection;

        graphics::RenderTarget* renderTarget = nullptr;
        std::unique_ptr<graphics::DepthStencilState> depthStencilState;
        std::uint32_t stencilReferenceValue = 0;

        bool clearColorBuffer = false;
        bool clearDepthBuffer = false;
        bool clearStencilBuffer = false;
        Color clearColor;
        float clearDepth = 1.0F;
        std::uint32_t clearStencil = 0;
    };
}

#endif // OUZEL_SCENE_CAMERA_HPP
