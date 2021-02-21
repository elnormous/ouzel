// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "Light.hpp"
#include "Camera.hpp"
#include "Layer.hpp"

namespace ouzel::scene
{
    Light::Light(Type initType):
        type(initType)
    {
    }

    Light::~Light()
    {
        if (layer) layer->removeLight(*this);
    }

    void Light::setLayer(Layer* newLayer)
    {
        if (layer) layer->addLight(*this);

        Component::setLayer(newLayer);

        if (layer) layer->removeLight(*this);
    }
}
