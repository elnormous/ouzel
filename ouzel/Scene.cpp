// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Scene.h"
#include "Layer.h"
#include "Camera.h"

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
            lock();

            if (reorder)
            {
                std::sort(layers.begin(), layers.end(), [](LayerPtr a, LayerPtr b) {
                    return a->getOrder() > b->getOrder();
                });

                reorder = false;
            }

            for (LayerPtr layer : layers)
            {
                if (!layer->remove)
                {
                    layer->draw();
                }
            }

            unlock();
        }

        void Scene::addLayer(const LayerPtr& layer)
        {
            if (locked)
            {
                layerAddList.insert(layer);
            }
            else if (!hasLayer(layer) && !layer->getScene())
            {
                layer->remove = false;
                layers.push_back(layer);
                layer->addToScene(shared_from_this());

                if (CameraPtr camera = layer->getCamera())
                {
                    camera->recalculateProjection();
                }
            }
        }

        void Scene::removeLayer(const LayerPtr& layer)
        {
            if (locked)
            {
                layer->remove = true;
                layerRemoveList.insert(layer);
            }
            else
            {
                std::vector<LayerPtr>::iterator i = std::find(layers.begin(), layers.end(), layer);

                if (i != layers.end())
                {
                    layer->removeFromScene();
                    layers.erase(i);
                }
            }
        }

        bool Scene::hasLayer(const LayerPtr& layer) const
        {
            std::vector<LayerPtr>::const_iterator i = std::find(layers.begin(), layers.end(), layer);

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

        void Scene::reorderLayers()
        {
            reorder = true;
        }

        void Scene::lock()
        {
            ++locked;
        }

        void Scene::unlock()
        {
            if (--locked == 0)
            {
                if (!layerAddList.empty())
                {
                    for (const LayerPtr& layer : layerAddList)
                    {
                        addLayer(layer);
                    }
                    layerAddList.clear();
                }

                if (!layerRemoveList.empty())
                {
                    for (const LayerPtr& layer : layerRemoveList)
                    {
                        removeLayer(layer);
                    }
                    layerRemoveList.clear();
                }
            }
        }

        NodePtr Scene::pickNode(const Vector2& position) const
        {
            for (std::vector<LayerPtr>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
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

            for (std::vector<LayerPtr>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::set<NodePtr> nodes = (*i)->pickNodes(edges);

                result.insert(nodes.begin(), nodes.end());
            }

            return result;
        }

    } // namespace scene
} // namespace ouzel
