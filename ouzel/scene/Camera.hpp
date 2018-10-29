// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
#include "scene/Component.hpp"
#include "math/MathUtils.hpp"
#include "math/Rect.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        class Layer;

        class Camera: public Component
        {
            friend Layer;
        public:
            static const uint32_t CLASS = Component::CAMERA;

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

            explicit Camera(Matrix4 initProjection);
            explicit Camera(const Size2& initTargetContentSize = Size2(), ScaleMode initScaleMode = ScaleMode::NONE);
            explicit Camera(float initFov, float initNearPlane = 1.0F, float initFarPlane = 100.0F);
            virtual ~Camera();

            void setProjectionMode(ProjectionMode newProjectionMode) { projectionMode = newProjectionMode; }
            inline ProjectionMode getProjectionMode() const { return projectionMode; }

            void setFOV(float newFOV) { fov = newFOV; }
            inline float getFOV() const { return fov; }

            void setNearPlane(float newNearPlane) { nearPlane = newNearPlane; }
            inline float getNearPlane() const { return nearPlane; }

            void setFarPlane(float newFarPlane) { farPlane = newFarPlane; }
            inline float getFarPlane() const { return farPlane; }

            const Matrix4& getProjection() const { return projection; }
            void recalculateProjection();

            const Matrix4& getViewProjection() const;
            const Matrix4& getRenderViewProjection() const;
            const Matrix4& getInverseViewProjection() const;

            Vector3 convertNormalizedToWorld(const Vector2& normalizedPosition) const;
            Vector2 convertWorldToNormalized(const Vector3& worldPosition) const;

            bool checkVisibility(const Matrix4& boxTransform, const Box3& box) const;

            void setViewport(const Rect& newViewport);
            inline const Rect& getViewport() const { return viewport; }
            inline const Rect& getRenderViewport() const { return renderViewport; }

            void setScaleMode(ScaleMode newScaleMode);
            inline ScaleMode getScaleMode() const { return scaleMode; }

            void setTargetContentSize(const Size2& newTargetContentSize);
            inline const Size2& getTargetContentSize() const { return targetContentSize; }

            inline const Size2& getContentSize() const { return contentSize; }
            inline const Vector2& getContentScale() const { return contentScale; }
            inline const Vector2& getContentPosition() const { return contentPosition; }

            void setRenderTarget(const std::shared_ptr<graphics::Texture>& newRenderTarget);
            inline const std::shared_ptr<graphics::Texture>& getRenderTarget() const { return renderTarget; }

            inline bool getDepthWrite() const { return depthWrite; }
            inline void setDepthWrite(bool newDepthWrite) { depthWrite = newDepthWrite; }
            inline bool getDepthTest() const { return depthTest; }
            inline void setDepthTest(bool newDepthTest) { depthTest = newDepthTest; }

            inline bool getWireframe() const { return wireframe; }
            inline void setWireframe(bool newWireframe) { wireframe = newWireframe; }

        protected:
            void setActor(Actor* newActor) override;
            void setLayer(Layer* newLayer) override;

            void updateTransform() override;
            void calculateViewProjection() const;

            ProjectionMode projectionMode;
            float fov = TAU / 6.0F;
            float nearPlane = 1.0F;
            float farPlane = 100.0F;

            Matrix4 projection;

            Rect viewport = Rect(0.0F, 0.0F, 1.0F, 1.0F);
            Rect renderViewport;
            Size2 targetContentSize;

            ScaleMode scaleMode = ScaleMode::NONE;
            Size2 contentSize;
            Vector2 contentScale;
            Vector2 contentPosition;

            bool depthWrite = false;
            bool depthTest = false;
            bool wireframe = false;

            mutable bool viewProjectionDirty = true;
            mutable Matrix4 viewProjection;
            mutable Matrix4 renderViewProjection;

            mutable bool inverseViewProjectionDirty = true;
            mutable Matrix4 inverseViewProjection;

            std::shared_ptr<graphics::Texture> renderTarget;
        };
    } // namespace scene
} // namespace ouzel
