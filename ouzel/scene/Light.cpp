// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Light.hpp"
#include "Camera.hpp"
#include "Layer.hpp"

namespace ouzel
{
    namespace scene
    {
        Light::Light():
            Component(CLASS)
        {
        }

        Light::Light(Type initType):
            Component(CLASS), type(initType)
        {
        }

        Light::~Light()
        {
            if (layer) layer->removeLight(this);
        }

        void Light::setLayer(Layer* newLayer)
        {
            if (layer) layer->addLight(this);

            Component::setLayer(newLayer);

            if (layer) layer->removeLight(this);
        }
    } // namespace scene
} // namespace ouzel
