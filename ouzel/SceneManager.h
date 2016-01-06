// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
#include <vector>
#include <memory>
#include "Noncopyable.h"
#include "Vector2.h"
#include "Rectangle.h"

namespace ouzel
{
    class Engine;
    class Camera;
    class Node;
    class Scene;
    
    class SceneManager: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~SceneManager();
        
        void update(float delta);
        
        void draw();
        
        void setScene(std::shared_ptr<Scene> const& scene);
        std::shared_ptr<Scene> getScene() const { return _scene; }
        
        virtual void recalculateProjection();
        
    protected:
        SceneManager();
        
        std::shared_ptr<Scene> _scene;
    };
}
