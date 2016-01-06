// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
#include "NodeContainer.h"
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
        Layer();
        virtual ~Layer();
        
        virtual void update(float delta);
        virtual void draw();
        
        void addNode(Node* node);
        void removeNode(Node* node);
        void reorderNodes();
        
        Camera* getCamera() const { return _camera; }
        void setCamera(Camera* camera);
        
        Node* pickNode(const Vector2& position);
        std::set<Node*> pickNodes(const Rectangle& rectangle);
        
        Vector2 screenToWorldLocation(const Vector2& position);
        Vector2 worldToScreenLocation(const Vector2& position);
        
        const Matrix4& getProjection() const { return _projection; }
        virtual void recalculateProjection();
        
        int32_t getOrder() const { return _order; }
        void setOrder(int32_t order);
        
        Scene* getScene() const { return _scene; }
        
    protected:
        virtual void addToScene(Scene* scene);
        virtual void removeFromScene();
        
        SharedPtr<Camera> _camera;
        std::vector<SharedPtr<Node>> _nodes;
        bool _reorderNodes = false;
        
        Matrix4 _projection;
        Matrix4 _inverseProjection;
        
        Scene* _scene = nullptr;
        int32_t _order = 0;
    };
}
