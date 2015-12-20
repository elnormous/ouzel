// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Node.h"

namespace ouzel
{
    class Scene;
    
    class Camera: public Node
    {
    public:
        Camera(Scene* scene);
        virtual ~Camera();
        
        float getZoom() const { return _zoom; }
        void setZoom(float zoom);
        
    protected:
        virtual void updateTransform() override;
        
        float _zoom = 1.0f;
    };
}
