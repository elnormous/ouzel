// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Scene.h"
#include "Layer.h"
#include "Camera.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace scene
    {
        Scene::Scene()
        {

        }

        Scene::~Scene()
        {

        }

        void Scene::draw()
        {
            layers.sort([](LayerPtr a, LayerPtr b) {
                return a->getOrder() > b->getOrder();
            });

            for (LayerPtr layer : layers)
            {
                layer->draw();
            }
        }

        void Scene::addLayer(const LayerPtr& layer)
        {
            if (!hasLayer(layer))
            {
                layers.push_back(layer);

                if (CameraPtr camera = layer->getCamera())
                {
                    camera->recalculateProjection();
                }
            }
        }

        void Scene::removeLayer(const LayerPtr& layer)
        {
            std::list<LayerPtr>::iterator i = std::find(layers.begin(), layers.end(), layer);

            if (i != layers.end())
            {
                layers.erase(i);
            }
        }

        void Scene::removeAllLayers()
        {
            layers.clear();
        }

        bool Scene::hasLayer(const LayerPtr& layer) const
        {
            std::list<LayerPtr>::const_iterator i = std::find(layers.begin(), layers.end(), layer);

            return i != layers.end();
        }

        void Scene::recalculateProjection()
        {
            for (LayerPtr layer : layers)
            {
                if (CameraPtr camera = layer->getCamera())
                {
                    camera->recalculateProjection();
                }
            }
        }

        NodePtr Scene::pickNode(const Vector2& position) const
        {
            for (std::list<LayerPtr>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
            {
                LayerPtr layer = *i;
                CameraPtr camera = layer->getCamera();

                if (camera)
                {
                    Vector2 worldPosition = camera->convertScreenToWorld(position);

                    if (NodePtr result = layer->pickNode(worldPosition))
                    {
                        return result;
                    }
                }
            }

            return nullptr;
        }

        std::set<NodePtr> Scene::pickNodes(const std::vector<Vector2>& edges) const
        {
            std::set<NodePtr> result;

            for (std::list<LayerPtr>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::set<NodePtr> nodes = (*i)->pickNodes(edges);

                result.insert(nodes.begin(), nodes.end());
            }

            return result;
        }

    } // namespace scene
} // namespace ouzel
