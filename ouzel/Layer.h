// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <set>
#include "Types.h"
#include "NodeContainer.h"
#include "Size2.h"
#include "Matrix4.h"
#include "Vector2.h"
#include "Rectangle.h"

namespace ouzel
{
    class Camera;
    class Scene;
    
    class Layer: public NodeContainer
    {
        friend Scene;
    public:
        
        enum class ScaleMode
        {
            None,
            ExactFit,
            NoBorder,
            ShowAll
        };
        
        Layer();
        virtual ~Layer();
        
        virtual void update(float delta);
        virtual void draw();
        
        virtual bool addChild(const NodePtr& node) override;
        
        void addNode(const NodePtr& node);
        void removeNode(const NodePtr& node);
        void reorderNodes();
        
        const CameraPtr& getCamera() const { return _camera; }
        void setCamera(const CameraPtr& camera);
        
        NodePtr pickNode(const Vector2& position);
        std::set<NodePtr> pickNodes(const Rectangle& rectangle);
        
        Vector2 screenToWorldLocation(const Vector2& position);
        Vector2 worldToScreenLocation(const Vector2& position);
        
        virtual void setScaleMode(ScaleMode scaleMode);
        virtual ScaleMode getScaleMode() const { return _scaleMode; }
        
        virtual void setDesignSize(const Size2& designSize);
        virtual const Size2& getDesignSize() const { return _designSize; }
        
        const Matrix4& getProjection() const { return _projection; }
        virtual void recalculateProjection();
        
        int32_t getOrder() const { return _order; }
        void setOrder(int32_t order);
        
        ScenePtr getScene() const { return _scene.lock(); }
        
    protected:
        virtual void addToScene(const ScenePtr& scene);
        virtual void removeFromScene();
        
        void lock();
        void unlock();
        
        CameraPtr _camera;
        std::vector<NodePtr> _nodes;
        bool _reorderNodes = false;
        
        ScaleMode _scaleMode = ScaleMode::None;
        Size2 _designSize;
        
        Matrix4 _projection;
        Matrix4 _inverseProjection;
        
        SceneWeakPtr _scene;
        int32_t _order = 0;
        
        std::set<NodePtr> _nodeDeleteList;
        bool _locked = false;
    };
}
