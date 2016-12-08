// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Types.h"
#include "scene/Node.h"
#include "math/MathUtils.h"
#include "math/Rectangle.h"

namespace ouzel
{
    namespace scene
    {
        class Layer;

        class Camera: public Node
        {
            friend Layer;
        public:
            enum class Type
            {
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

            Camera(Type aType = Type::ORTHOGRAPHIC, float aFov = 60.0f);
            virtual ~Camera();

            const Matrix4& getProjection() const { return projection; }
            void recalculateProjection();

            const Matrix4& getViewProjection() const;

            Vector3 convertNormalizedToWorld(const Vector2& position) const;
            Vector2 convertWorldToNormalized(const Vector3& position) const;

            bool checkVisibility(const Matrix4& boxTransform, const AABB2& boundingBox) const;

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

            void setRenderTarget(const graphics::RenderTargetPtr& newRenderTarget);
            const graphics::RenderTargetPtr& getRenderTarget() const { return renderTarget; }

            bool getWireframe() const { return wireframe; }
            void setWireframe(bool newWireframe) { wireframe = newWireframe; }

        protected:
            virtual void calculateLocalTransform() const override;

            Type type;
            float fov;

            Matrix4 projection;
            Matrix4 inverseProjection;

            Rectangle viewport = Rectangle(0.0f, 0.0f, 1.0f, 1.0f);
            Rectangle renderViewport;
            Size2 targetContentSize;

            ScaleMode scaleMode = ScaleMode::NONE;
            Size2 contentSize;
            Vector2 contentScale;
            Vector2 contentPosition;

            mutable bool viewProjectionDirty = false;
            mutable Matrix4 viewProjection;

            Layer* layer = nullptr;

            graphics::RenderTargetPtr renderTarget;

            bool wireframe = false;
        };
    } // namespace scene
} // namespace ouzel
