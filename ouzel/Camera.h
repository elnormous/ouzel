//
//  Camera.h
//  Ouzel
//
//  Created by Elviss Strazdins on 15/03/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

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
