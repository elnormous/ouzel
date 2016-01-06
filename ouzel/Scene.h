// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
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
        
        void addLayer(std::shared_ptr<Layer> layer);
        void removeLayer(std::shared_ptr<Layer> layer);
        bool hasLayer(std::shared_ptr<Layer> layer) const;
        const std::vector<std::shared_ptr<Layer>>& getLayers() const { return _layers; }
        
        virtual void recalculateProjection();
        
        virtual void reorderLayers();
        
    protected:
        std::vector<std::shared_ptr<Layer>> _layers;
        bool _reorderLayers = false;
    };
}
