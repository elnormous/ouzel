// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Scene.h"
#include "Layer.h"

namespace ouzel
{
    Scene::Scene()
    {
        
    }
    
    Scene::~Scene()
    {
        for (SharedPtr<Layer> layer : _layers)
        {
            layer->_scene = nullptr;
        }
    }
    
    void Scene::update(float delta)
    {
        for (SharedPtr<Layer> layer : _layers)
        {
            layer->update(delta);
        }
    }
    
    void Scene::draw()
    {
        if (_reorderLayers)
        {
            std::sort(_layers.begin(), _layers.end(), [](Layer* a, Layer* b) {
                return a->getOrder() > b->getOrder();
            });
            
            _reorderLayers = false;
        }
        
        for (SharedPtr<Layer> layer : _layers)
        {
            layer->draw();
        }
    }
    
    void Scene::addLayer(Layer* layer)
    {
        if (!hasLayer(layer) && layer->getScene() == nullptr)
        {
            _layers.push_back(layer);
            layer->addToScene(this);
            layer->recalculateProjection();
        }
    }
    
    void Scene::removeLayer(Layer* layer)
    {
        std::vector<SharedPtr<Layer>>::iterator i = std::find(_layers.begin(), _layers.end(), layer);
        
        if (i != _layers.end())
        {
            layer->removeFromScene();
            _layers.erase(i);
        }
    }
    
    bool Scene::hasLayer(Layer* layer) const
    {
        std::vector<SharedPtr<Layer>>::const_iterator i = std::find(_layers.begin(), _layers.end(), layer);
        
        return i != _layers.end();
    }
    
    void Scene::recalculateProjection()
    {
        for (SharedPtr<Layer> layer : _layers)
        {
            layer->recalculateProjection();
        }
    }
    
    void Scene::reorderLayers()
    {
        _reorderLayers = true;
    }
}
