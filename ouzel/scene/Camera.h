// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Node.h"

namespace ouzel
{
    namespace scene
    {
        class Layer;

        class Camera: public Node
        {
            friend Layer;
        public:
            enum class ScaleMode
            {
                NONE,
                EXACT_FIT,
                NO_BORDER,
                SHOW_ALL
            };

            Camera();
            virtual ~Camera();

            const Matrix4& getProjection() const { return projection; }
            void recalculateProjection();

            const Matrix4& getViewProjection() const;

            Vector2 convertNormalizedToWorld(const Vector2& position) const;
            Vector2 convertWorldToNormalized(const Vector2& position) const;

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

            virtual void addToLayer(Layer* newLayer);
            virtual void removeFromLayer();

            void setRenderTarget(const graphics::RenderTargetPtr& newRenderTarget);
            const graphics::RenderTargetPtr& getRenderTarget() const { return renderTarget; }
            
        protected:
            virtual void calculateLocalTransform() const override;

            Matrix4 projection = Matrix4::IDENTITY;
            Matrix4 inverseProjection = Matrix4::IDENTITY;

            Rectangle viewport = Rectangle(0.0f, 0.0f, 1.0f, 1.0f);
            Rectangle renderViewport;
            Size2 targetContentSize;

            ScaleMode scaleMode = ScaleMode::NONE;
            Size2 contentSize;
            Vector2 contentScale;
            Vector2 contentPosition;

            mutable bool viewProjectionDirty = false;
            mutable Matrix4 viewProjection = Matrix4::IDENTITY;

            Layer* layer = nullptr;

            graphics::RenderTargetPtr renderTarget;
        };
    } // namespace scene
} // namespace ouzel
