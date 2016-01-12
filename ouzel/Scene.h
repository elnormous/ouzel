// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include <set>
#include "Types.h"
#include "Noncopyable.h"

namespace ouzel
{
    class Layer;
    
    class Scene: public Noncopyable, public std::enable_shared_from_this<Scene>
    {
    public:
        Scene();
        virtual ~Scene();
        
        virtual void update(float delta);
        virtual void draw();
        
        void addLayer(LayerPtr const& layer);
        void removeLayer(LayerPtr const& layer);
        bool hasLayer(LayerPtr const& layer) const;
        const std::vector<LayerPtr>& getLayers() const { return _layers; }
        
        virtual void recalculateProjection();
        
        virtual void reorderLayers();
        
    protected:
        void lock();
        void unlock();
        
        std::vector<LayerPtr> _layers;
        bool _reorderLayers = false;
        
        std::set<LayerPtr> _layerDeleteList;
        bool _locked = false;
    };
}
