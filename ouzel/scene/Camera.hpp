// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_CAMERA_HPP
#define OUZEL_SCENE_CAMERA_HPP

#include <memory>
#include "scene/Component.hpp"
#include "math/MathUtils.hpp"
#include "math/Rect.hpp"
#include "graphics/DepthStencilState.hpp"
#include "graphics/RenderTarget.hpp"

namespace ouzel
{
    namespace scene
    {
        class Layer;

        class Camera: public Component
        {
            friend Layer;
        public:
            enum class ProjectionMode
            {
                Custom,
                Orthographic,
                Perspective
            };

            enum class ScaleMode
            {
                NoScale,
                ExactFit,
                NoBorder,
                ShowAll
            };

            explicit Camera(const Matrix4F& initProjection);
            explicit Camera(const Size2F& initTargetContentSize = Size2F(), ScaleMode initScaleMode = ScaleMode::NoScale);
            explicit Camera(float initFov, float initNearPlane = 1.0F, float initFarPlane = 100.0F);
            ~Camera() override;

            inline auto getProjectionMode() const noexcept { return projectionMode; }
            inline void setProjectionMode(ProjectionMode newProjectionMode) { projectionMode = newProjectionMode; }

            inline auto getFOV() const noexcept { return fov; }
            inline void setFOV(float newFOV) { fov = newFOV; }

            inline auto getNearPlane() const noexcept { return nearPlane; }
            inline void setNearPlane(float newNearPlane) { nearPlane = newNearPlane; }

            inline auto getFarPlane() const noexcept { return farPlane; }
            inline void setFarPlane(float newFarPlane) { farPlane = newFarPlane; }

            inline const Matrix4F& getProjection() const noexcept { return projection; }
            void recalculateProjection();

            const Matrix4F& getViewProjection() const;
            const Matrix4F& getRenderViewProjection() const;
            const Matrix4F& getInverseViewProjection() const;

            Vector3F convertNormalizedToWorld(const Vector2F& normalizedPosition) const;
            Vector2F convertWorldToNormalized(const Vector3F& worldPosition) const;

            bool checkVisibility(const Matrix4F& boxTransform, const Box3F& box) const;

            inline const RectF& getViewport() const noexcept { return viewport; }
            inline const RectF& getRenderViewport() const noexcept { return renderViewport; }
            void setViewport(const RectF& newViewport);

            inline auto getScaleMode() const noexcept { return scaleMode; }
            void setScaleMode(ScaleMode newScaleMode);

            inline const Size2F& getTargetContentSize() const noexcept { return targetContentSize; }
            void setTargetContentSize(const Size2F& newTargetContentSize);

            inline const Size2F& getContentSize() const noexcept { return contentSize; }
            inline const Vector2F& getContentScale() const noexcept { return contentScale; }
            inline const Vector2F& getContentPosition() const noexcept { return contentPosition; }

            inline const std::shared_ptr<graphics::RenderTarget>& getRenderTarget() const noexcept { return renderTarget; }
            void setRenderTarget(const std::shared_ptr<graphics::RenderTarget>& newRenderTarget);

            inline auto getDepthTest() const noexcept { return depthTest; }
            void setDepthTest(bool newDepthTest);
            inline const std::shared_ptr<graphics::DepthStencilState>& getDepthStencilState() const noexcept { return depthStencilState; }

            inline auto getStencilReferenceValue() const noexcept { return stencilReferenceValue; }
            inline void setStencilReferenceValue(uint32_t newStencilReferenceValue) { stencilReferenceValue = newStencilReferenceValue; }

            inline auto getWireframe() const noexcept { return wireframe; }
            inline void setWireframe(bool newWireframe) { wireframe = newWireframe; }

            inline auto getClearColorBuffer() const noexcept { return clearColorBuffer; }
            inline void setClearColorBuffer(bool clear) { clearColorBuffer = clear; }

            inline auto getClearDepthBuffer() const noexcept { return clearDepthBuffer; }
            inline void setClearDepthBuffer(bool clear) { clearDepthBuffer = clear; }

            inline auto getClearStencilBuffer() const noexcept { return clearStencilBuffer; }
            inline void setClearStencilBuffer(bool clear) { clearStencilBuffer = clear; }

            inline auto getClearColor() const noexcept { return clearColor; }
            inline void setClearColor(Color color) { clearColor = color; }

            inline auto getClearDepth() const noexcept { return clearDepth; }
            inline void setClearDepth(float depth) { clearDepth = depth; }

            inline auto getClearStencil() const noexcept { return clearStencil; }
            inline void setClearDepth(uint32_t stencil) { clearStencil = stencil; }

        private:
            void setActor(Actor* newActor) override;
            void setLayer(Layer* newLayer) override;

            void updateTransform() override;
            void calculateViewProjection() const;

            ProjectionMode projectionMode;
            float fov = tau<float> / 6.0F;
            float nearPlane = 1.0F;
            float farPlane = 100.0F;

            Matrix4F projection;

            RectF viewport = RectF(0.0F, 0.0F, 1.0F, 1.0F);
            RectF renderViewport;
            Size2F targetContentSize;

            ScaleMode scaleMode = ScaleMode::NoScale;
            Size2F contentSize;
            Vector2F contentScale;
            Vector2F contentPosition;

            bool depthTest = false;
            bool wireframe = false;

            mutable bool viewProjectionDirty = true;
            mutable Matrix4F viewProjection;
            mutable Matrix4F renderViewProjection;

            mutable bool inverseViewProjectionDirty = true;
            mutable Matrix4F inverseViewProjection;

            std::shared_ptr<graphics::RenderTarget> renderTarget;
            std::shared_ptr<graphics::DepthStencilState> depthStencilState;
            uint32_t stencilReferenceValue = 0;

            bool clearColorBuffer = false;
            bool clearDepthBuffer = false;
            bool clearStencilBuffer = false;
            Color clearColor;
            float clearDepth = 1.0F;
            uint32_t clearStencil = 0;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_CAMERA_HPP
