// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Scene.h"
#include "Layer.h"

namespace ouzel
{
    Scene::Scene()
    {
        
    }
    
    Scene::~Scene()
    {
        
    }
    
    void Scene::update(float delta)
    {
        for (AutoPtr<Layer> layer : _layers)
        {
            layer->update(delta);
        }
    }
    
    void Scene::draw()
    {
        for (AutoPtr<Layer> layer : _layers)
        {
            layer->draw();
        }
    }
    
    void Scene::addLayer(Layer* layer)
    {
        if (!hasLayer(layer))
        {
            _layers.push_back(layer);
            layer->recalculateProjection();
        }
    }
    
    void Scene::removeLayer(Layer* layer)
    {
        std::vector<AutoPtr<Layer>>::iterator i = std::find(_layers.begin(), _layers.end(), layer);
        
        if (i != _layers.end())
        {
            _layers.erase(i);
        }
    }
    
    bool Scene::hasLayer(Layer* layer) const
    {
        std::vector<AutoPtr<Layer>>::const_iterator i = std::find(_layers.begin(), _layers.end(), layer);
        
        return i != _layers.end();
    }
    
    void Scene::recalculateProjection()
    {
        for (AutoPtr<Layer> layer : _layers)
        {
            layer->recalculateProjection();
        }
    }
}
