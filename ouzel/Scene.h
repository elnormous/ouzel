// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
#include <vector>
#include "AutoPtr.h"
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Vector2.h"
#include "Rectangle.h"

namespace ouzel
{
    class Engine;
    class Camera;
    class Node;
    
    class Scene: public Noncopyable, public ReferenceCounted
    {
    public:
        static Scene* getInstance();

        Scene();
        virtual ~Scene();
        
        bool init();
        
        void update(float delta);
        
        void addNode(Node* node);
        void removeNode(Node* node);
        void reorderNodes();
        
        Node* getRootNode() const { return _rootNode; }
        
        Camera* getCamera() const { return _camera; }
        void setCamera(Camera* camera);

        Node* pickNode(const Vector2& position);
        std::set<Node*> pickNodes(const Rectangle& rectangle);
        
        void drawAll();
        
    protected:
        AutoPtr<Node> _rootNode;
        AutoPtr<Camera> _camera;
        std::vector<AutoPtr<Node>> _nodes;
        bool _reorderNodes = false;
    };
}
