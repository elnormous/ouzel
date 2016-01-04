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
    
    class Layer: public NodeContainer
    {
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
        
    protected:
        AutoPtr<Camera> _camera;
        std::vector<AutoPtr<Node>> _nodes;
        bool _reorderNodes = false;
        
        Matrix4 _projection;
        Matrix4 _inverseProjection;
    };
}
