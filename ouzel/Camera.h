// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Node.h"

namespace ouzel
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
        
        float getZoom() const { return _zoom; }
        void setZoom(float zoom);
        
        const Matrix4& getProjection() const { return _projection; }
        virtual void recalculateProjection();
        
        const Matrix4& getViewProjection() const;
        
        Vector2 screenToWorldLocation(const Vector2& position);
        Vector2 worldToScreenLocation(const Vector2& position);
        
        Vector2 projectPoint(const Vector3& src) const;
        
        virtual void setScaleMode(ScaleMode scaleMode);
        virtual ScaleMode getScaleMode() const { return _scaleMode; }
        
        virtual void setTargetContentSize(const Size2& targetContentSize);
        const Size2& getTargetContentSize() const { return _targetContentSize; }
        
        virtual const Size2& getContentSize() const { return _contentSize; }
        virtual const Vector2& getContentScale() const { return _contentScale; }
        
    protected:
        virtual void calculateLocalTransform() const override;
        
        float _zoom = 1.0f;
        
        Matrix4 _projection;
        Matrix4 _inverseProjection;
        
        Size2 _targetContentSize;
        
        ScaleMode _scaleMode = ScaleMode::None;
        Size2 _contentSize;
        Vector2 _contentScale;
        
        bool _viewProjectionDirty = false;
        mutable Matrix4 _viewProjection;
    };
}
