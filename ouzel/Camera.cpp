// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Camera.h"

namespace ouzel
{
    Camera::Camera()
    {
        
    }

    Camera::~Camera()
    {
        
    }

    void Camera::updateTransform(const Matrix4& parentTransform)
    {
        if (_transformDirty)
        {
            Matrix4 translation;
            translation.translate(-_position.x, -_position.y, 0.0f);
            
            Matrix4 rotation;
            rotation.rotate(Vector3(0.0f, 0.0f, -1.0f), -_rotation);
            
            Matrix4 scale;
            scale.scale(_zoom);
            
            _transform = parentTransform * scale * rotation * translation;
            _inverseTransform = _transform;
            _inverseTransform.invert();
        }
        
        for (AutoPtr<Node> child : _children)
        {
            child->updateTransform(_transform);
        }
    }

    void Camera::setZoom(float zoom)
    {
        _zoom = zoom;
        
        if (_zoom < 0.1f)
        {
            _zoom = 0.1f;
        }
        
        markTransformDirty();
    }
}
