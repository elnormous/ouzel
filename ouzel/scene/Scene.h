// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
#include <list>
#include <set>
#include <cstdint>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "math/Rectangle.h"

namespace ouzel
{
    namespace scene
    {
        class Scene: public Noncopyable, public std::enable_shared_from_this<Scene>
        {
        public:
            Scene();
            virtual ~Scene();

            virtual void draw();

            void addLayer(const LayerPtr& layer);
            void removeLayer(const LayerPtr& layer);
            void removeAllLayers();
            bool hasLayer(const LayerPtr& layer) const;
            const std::list<LayerPtr>& getLayers() const { return layers; }

            virtual void recalculateProjection();

            NodePtr pickNode(const Vector2& position) const;
            std::set<NodePtr> pickNodes(const std::vector<Vector2>& edges) const;

        protected:
            std::list<LayerPtr> layers;
        };
    } // namespace scene
} // namespace ouzel
