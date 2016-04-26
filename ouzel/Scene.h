// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include <set>
#include <cstdint>
#include "Types.h"
#include "Noncopyable.h"
#include "Rectangle.h"

namespace ouzel
{
    namespace scene
    {
        class Layer;

        class Scene: public Noncopyable, public std::enable_shared_from_this<Scene>
        {
        public:
            Scene();
            virtual ~Scene();

            virtual void draw();

            void addLayer(const LayerPtr& layer);
            void removeLayer(const LayerPtr& layer);
            bool hasLayer(const LayerPtr& layer) const;
            const std::vector<LayerPtr>& getLayers() const { return _layers; }

            virtual void recalculateProjection();

            virtual void reorderLayers();

            NodePtr pickNode(const Vector2& position) const;
            std::set<NodePtr> pickNodes(const std::vector<Vector2>& edges) const;

        protected:
            void lock();
            void unlock();

            std::vector<LayerPtr> _layers;
            bool _reorderLayers = false;

            std::set<LayerPtr> _layerAddList;
            std::set<LayerPtr> _layerRemoveList;
            int32_t _locked = 0;
        };
    } // namespace scene
} // namespace ouzel
