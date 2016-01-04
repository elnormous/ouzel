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
    class Scene;
    
    class SceneManager: public Noncopyable, public ReferenceCounted
    {
    public:
        static SceneManager* getInstance();

        SceneManager();
        virtual ~SceneManager();
        
        void update(float delta);
        
        void draw();
        
        void setScene(Scene* scene);
        Scene* getScene() const { return _scene; }
        
    protected:
        AutoPtr<Scene> _scene;
    };
}
