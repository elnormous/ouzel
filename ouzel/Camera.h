// Copyright (C) 2015 Elviss Strazdins
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
        
        virtual void updateTransform(const Matrix4& parentTransform) override;
        
    protected:
        float _zoom = 1.0f;
    };
}
