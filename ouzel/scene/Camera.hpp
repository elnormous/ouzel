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

        class Camera final: public Component
        {
            friend Layer;
        public:
            static constexpr uint32_t CLASS = Component::CAMERA;

            enum class ProjectionMode
            {
                CUSTOM,
                ORTHOGRAPHIC,
                PERSPECTIVE
            };

            enum class ScaleMode
            {
                NONE,
                EXACT_FIT,
                NO_BORDER,
                SHOW_ALL
            };

            explicit Camera(const Matrix4F& initProjection);
            explicit Camera(const Size2F& initTargetContentSize = Size2F(), ScaleMode initScaleMode = ScaleMode::NONE);
            explicit Camera(float initFov, float initNearPlane = 1.0F, float initFarPlane = 100.0F);
            ~Camera();

            inline ProjectionMode getProjectionMode() const { return projectionMode; }
            void setProjectionMode(ProjectionMode newProjectionMode) { projectionMode = newProjectionMode; }

            inline float getFOV() const { return fov; }
            void setFOV(float newFOV) { fov = newFOV; }

            inline float getNearPlane() const { return nearPlane; }
            void setNearPlane(float newNearPlane) { nearPlane = newNearPlane; }

            inline float getFarPlane() const { return farPlane; }
            void setFarPlane(float newFarPlane) { farPlane = newFarPlane; }

            const Matrix4F& getProjection() const { return projection; }
            void recalculateProjection();

            const Matrix4F& getViewProjection() const;
            const Matrix4F& getRenderViewProjection() const;
            const Matrix4F& getInverseViewProjection() const;

            Vector3F convertNormalizedToWorld(const Vector2F& normalizedPosition) const;
            Vector2F convertWorldToNormalized(const Vector3F& worldPosition) const;

            bool checkVisibility(const Matrix4F& boxTransform, const Box3F& box) const;

            inline const RectF& getViewport() const { return viewport; }
            inline const RectF& getRenderViewport() const { return renderViewport; }
            void setViewport(const RectF& newViewport);

            inline ScaleMode getScaleMode() const { return scaleMode; }
            void setScaleMode(ScaleMode newScaleMode);

            inline const Size2F& getTargetContentSize() const { return targetContentSize; }
            void setTargetContentSize(const Size2F& newTargetContentSize);

            inline const Size2F& getContentSize() const { return contentSize; }
            inline const Vector2F& getContentScale() const { return contentScale; }
            inline const Vector2F& getContentPosition() const { return contentPosition; }

            inline const std::shared_ptr<graphics::RenderTarget>& getRenderTarget() const { return renderTarget; }
            void setRenderTarget(const std::shared_ptr<graphics::RenderTarget>& newRenderTarget);

            inline bool getDepthTest() const { return depthTest; }
            void setDepthTest(bool newDepthTest);
            inline const std::shared_ptr<graphics::DepthStencilState>& getDepthStencilState() const { return depthStencilState; }

            uint32_t getStencilReferenceValue() const { return stencilReferenceValue; }
            void setStencilReferenceValue(uint32_t newStencilReferenceValue) { stencilReferenceValue = newStencilReferenceValue; }

            inline bool getWireframe() const { return wireframe; }
            inline void setWireframe(bool newWireframe) { wireframe = newWireframe; }

            inline bool getClearColorBuffer() const { return clearColorBuffer; }
            void setClearColorBuffer(bool clear) { clearColorBuffer = clear; }

            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }
            void setClearDepthBuffer(bool clear) { clearDepthBuffer = clear; }

            inline bool getClearStencilBuffer() const { return clearStencilBuffer; }
            void setClearStencilBuffer(bool clear) { clearStencilBuffer = clear; }

            inline Color getClearColor() const { return clearColor; }
            void setClearColor(Color color) { clearColor = color; }

            inline float getClearDepth() const { return clearDepth; }
            void setClearDepth(float depth) { clearDepth = depth; }

            inline uint32_t getClearStencil() const { return clearStencil; }
            void setClearDepth(uint32_t stencil) { clearStencil = stencil; }

        private:
            void setActor(Actor* newActor) override;
            void setLayer(Layer* newLayer) override;

            void updateTransform() override;
            void calculateViewProjection() const;

            ProjectionMode projectionMode;
            float fov = tau<float>() / 6.0F;
            float nearPlane = 1.0F;
            float farPlane = 100.0F;

            Matrix4F projection;

            RectF viewport = RectF(0.0F, 0.0F, 1.0F, 1.0F);
            RectF renderViewport;
            Size2F targetContentSize;

            ScaleMode scaleMode = ScaleMode::NONE;
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
