// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Light.hpp"
#include "Camera.hpp"
#include "Layer.hpp"

namespace ouzel
{
    namespace scene
    {
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
