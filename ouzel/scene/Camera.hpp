// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "scene/Component.hpp"
#include "math/MathUtils.hpp"
#include "math/Rectangle.hpp"
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
            static const uint32_t TYPE = Component::CAMERA;

            enum class Type
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

            Camera(Matrix4 initProjection);
            Camera(const Size2& initTargetContentSize = Size2(), ScaleMode initScaleMode = ScaleMode::NONE);
            Camera(float initFov, float initNearPlane = 1.0f, float initFarPlane = 100.0f);
            virtual ~Camera();

            void setType(Type newType) { type = newType; }
            Type getType() const { return type; }

            void setFOV(float newFOV) { fov = newFOV; }
            float getFOV() const { return fov; }

            void setNearPlane(float newNearPlane) { nearPlane = newNearPlane; }
            float getNearPlane() const { return nearPlane; }

            void setFarPlane(float newFarPlane) { farPlane = newFarPlane; }
            float getFarPlane() const { return farPlane; }

            const Matrix4& getProjection() const { return projection; }
            void recalculateProjection();

            const Matrix4& getViewProjection() const;
            const Matrix4& getRenderViewProjection() const;
            const Matrix4& getInverseViewProjection() const;

            Vector3 convertNormalizedToWorld(const Vector2& normalizedPosition) const;
            Vector2 convertWorldToNormalized(const Vector3& worldPosition) const;

            bool checkVisibility(const Matrix4& boxTransform, const Box3& box) const;

            void setViewport(const Rectangle& newViewport);
            const Rectangle& getViewport() const { return viewport; }
            const Rectangle& getRenderViewport() const { return renderViewport; }

            void setScaleMode(ScaleMode newScaleMode);
            ScaleMode getScaleMode() const { return scaleMode; }

            void setTargetContentSize(const Size2& newTargetContentSize);
            const Size2& getTargetContentSize() const { return targetContentSize; }

            const Size2& getContentSize() const { return contentSize; }
            const Vector2& getContentScale() const { return contentScale; }
            const Vector2& getContentPosition() const { return contentPosition; }

            void setRenderTarget(const std::shared_ptr<graphics::Texture>& newRenderTarget);
            const std::shared_ptr<graphics::Texture>& getRenderTarget() const { return renderTarget; }

            bool getDepthWrite() const { return depthWrite; }
            void setDepthWrite(bool newDepthWrite) { depthWrite = newDepthWrite; }
            bool getDepthTest() const { return depthTest; }
            void setDepthTest(bool newDepthTest) { depthTest = newDepthTest; }

            bool getWireframe() const { return wireframe; }
            void setWireframe(bool newWireframe) { wireframe = newWireframe; }

        protected:
            virtual void setActor(Actor* newActor) override;
            virtual void setLayer(Layer* newLayer) override;

            virtual void updateTransform() override;
            void calculateViewProjection() const;

            Type type;
            float fov = TAU / 6.0f;
            float nearPlane = 1.0f;
            float farPlane = 100.0f;

            Matrix4 projection;

            Rectangle viewport = Rectangle(0.0f, 0.0f, 1.0f, 1.0f);
            Rectangle renderViewport;
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
