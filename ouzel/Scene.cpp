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
        for (AutoPtr<Layer> layer : _layers)
        {
            layer->_scene = nullptr;
        }
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
        if (_reorderLayers)
        {
            std::sort(_layers.begin(), _layers.end(), [](Layer* a, Layer* b){
                return a->getOrder() < b->getOrder();
            });
            
            _reorderLayers = false;
        }
        
        for (std::vector<AutoPtr<Layer>>::reverse_iterator i = _layers.rbegin(); i != _layers.rend(); ++i)
        {
            Layer* layer = *i;
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
        std::vector<AutoPtr<Layer>>::iterator i = std::find(_layers.begin(), _layers.end(), layer);
        
        if (i != _layers.end())
        {
            layer->removeFromScene();
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
    
    void Scene::reorderLayers()
    {
        _reorderLayers = true;
    }
}
