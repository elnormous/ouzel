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
        Camera();
        virtual ~Camera();
        
        float getZoom() const { return _zoom; }
        void setZoom(float zoom);
        
    protected:
        virtual void calculateLocalTransform() const override;
        virtual void calculateTransform() const override;
        
        float _zoom = 1.0f;
    };
}
