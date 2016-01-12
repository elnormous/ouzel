// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <set>
#include "NodeContainer.h"
#include "Matrix4.h"
#include "Vector2.h"
#include "Rectangle.h"

namespace ouzel
{
    class Camera;
    class Scene;
    
    class Layer: public NodeContainer, public std::enable_shared_from_this<Layer>
    {
        friend Scene;
    public:
        Layer();
        virtual ~Layer();
        
        virtual void update(float delta);
        virtual void draw();
        
        virtual bool addChild(NodePtr const& node) override;
        virtual bool removeChild(NodePtr const& node) override;
        
        void addNode(NodePtr const& node);
        void removeNode(NodePtr const& node);
        void reorderNodes();
        
        CameraPtr const& getCamera() const { return _camera; }
        void setCamera(CameraPtr const& camera);
        
        NodePtr pickNode(const Vector2& position);
        std::set<NodePtr> pickNodes(const Rectangle& rectangle);
        
        Vector2 screenToWorldLocation(const Vector2& position);
        Vector2 worldToScreenLocation(const Vector2& position);
        
        const Matrix4& getProjection() const { return _projection; }
        virtual void recalculateProjection();
        
        int32_t getOrder() const { return _order; }
        void setOrder(int32_t order);
        
        ScenePtr getScene() const { return _scene.lock(); }
        
    protected:
        virtual void addToScene(ScenePtr const& scene);
        virtual void removeFromScene();
        
        void lock();
        void unlock();
        
        CameraPtr _camera;
        std::vector<NodePtr> _nodes;
        bool _reorderNodes = false;
        
        Matrix4 _projection;
        Matrix4 _inverseProjection;
        
        std::weak_ptr<Scene> _scene;
        int32_t _order = 0;
        
        std::set<NodePtr> _nodeDeleteList;
        bool _locked = false;
    };
}
