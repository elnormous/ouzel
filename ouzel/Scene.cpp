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

    }
    
    void Scene::update(float delta)
    {
        for (LayerPtr layer : _layers)
        {
            layer->update(delta);
        }
    }
    
    void Scene::draw()
    {
        if (_reorderLayers)
        {
            std::sort(_layers.begin(), _layers.end(), [](LayerPtr a, LayerPtr b) {
                return a->getOrder() > b->getOrder();
            });
            
            _reorderLayers = false;
        }
        
        for (LayerPtr layer : _layers)
        {
            layer->draw();
        }
    }
    
    void Scene::addLayer(const LayerPtr& layer)
    {
        if (!hasLayer(layer) && !layer->getScene())
        {
            _layers.push_back(layer);
            layer->addToScene(shared_from_this());
            layer->recalculateProjection();
            
        }
    }
    
    void Scene::removeLayer(const LayerPtr& layer)
    {
        if (_locked)
        {
            _layerDeleteList.insert(layer);
        }
        else
        {
            std::vector<LayerPtr>::iterator i = std::find(_layers.begin(), _layers.end(), layer);
            
            if (i != _layers.end())
            {
                layer->removeFromScene();
                _layers.erase(i);
            }
        }
    }
    
    bool Scene::hasLayer(const LayerPtr& layer) const
    {
        std::vector<LayerPtr>::const_iterator i = std::find(_layers.begin(), _layers.end(), layer);
        
        return i != _layers.end();
    }
    
    void Scene::recalculateProjection()
    {
        for (LayerPtr layer : _layers)
        {
            layer->recalculateProjection();
        }
    }
    
    void Scene::reorderLayers()
    {
        _reorderLayers = true;
    }
    
    void Scene::lock()
    {
        _locked = true;
    }
    
    void Scene::unlock()
    {
        _locked = false;
        
        for (const LayerPtr& layer : _layerDeleteList)
        {
            removeLayer(layer);
        }
        
        _layerDeleteList.clear();
    }
}
