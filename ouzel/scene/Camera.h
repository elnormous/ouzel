// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Node.h"

namespace ouzel
{
    namespace scene
    {
        class SceneManager;

        class Camera: public Node
        {
        public:
            enum class ScaleMode
            {
                None,
                ExactFit,
                NoBorder,
                ShowAll
            };

            Camera();
            virtual ~Camera();

            float getZoom() const { return zoom; }
            void setZoom(float newZoom);

            const Matrix4& getProjection() const { return projection; }
            virtual void recalculateProjection();

            const Matrix4& getViewProjection() const;

            Vector2 convertScreenToWorld(const Vector2& position);
            Vector2 convertWorldToScreen(const Vector2& position);

            Vector2 projectPoint(const Vector3& src) const;

            virtual void setScaleMode(ScaleMode newScaleMode);
            virtual ScaleMode getScaleMode() const { return scaleMode; }

            virtual void setTargetContentSize(const Size2& newTargetContentSize);
            const Size2& getTargetContentSize() const { return targetContentSize; }

            virtual const Size2& getContentSize() const { return contentSize; }
            virtual const Vector2& getContentScale() const { return contentScale; }

            virtual void addToLayer(const LayerWeakPtr& newLayer);
            virtual void removeFromLayer();

        protected:
            virtual void calculateLocalTransform() const override;

            float zoom = 1.0f;

            Matrix4 projection;
            Matrix4 inverseProjection;

            Size2 targetContentSize;

            ScaleMode scaleMode = ScaleMode::None;
            Size2 contentSize;
            Vector2 contentScale;

            bool viewProjectionDirty = false;
            mutable Matrix4 viewProjection;

            LayerWeakPtr layer;
        };
    } // namespace scene
} // namespace ouzel
