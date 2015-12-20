//
//  Scene.h
//  Ouzel
//
//  Created by Elviss Strazdins on 06/04/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

#pragma once

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
        Scene(Engine* engine);
        virtual ~Scene();
        
        bool init();
        
        Engine* getEngine() const { return _engine; }
        
        void addNode(Node* node);
        void removeNode(Node* node);
        void reorderNodes();
        
        Node* getRootNode() const { return _rootNode; }
        
        Camera* getCamera() const { return _camera; }
        void setCamera(Camera* camera);

        Node* pickNode(const Vector2& position);
        std::set<Node*> pickNodes(const Rectangle& rectangle);
        
        void drawAll();
        
    private:
        Engine* _engine;
        
        Node* _rootNode = nullptr;
        Camera* _camera = nullptr;
        std::vector<Node*> _nodes;
        bool _reorderNodes = false;
    };
}
