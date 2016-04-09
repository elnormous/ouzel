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

            if (_reorderLayers)
            {
                std::sort(_layers.begin(), _layers.end(), [](LayerPtr a, LayerPtr b) {
                    return a->getOrder() > b->getOrder();
                });

                _reorderLayers = false;
            }

            for (LayerPtr layer : _layers)
            {
                if (!layer->_remove)
                {
                    layer->draw();
                }
            }

            unlock();
        }

        void Scene::addLayer(const LayerPtr& layer)
        {
            if (_locked)
            {
                _layerAddList.insert(layer);
            }
            else if (!hasLayer(layer) && !layer->getScene())
            {
                layer->_remove = false;
                _layers.push_back(layer);
                layer->addToScene(shared_from_this());

                if (CameraPtr camera = layer->getCamera())
                {
                    camera->recalculateProjection();
                }
            }
        }

        void Scene::removeLayer(const LayerPtr& layer)
        {
            if (_locked)
            {
                layer->_remove = true;
                _layerRemoveList.insert(layer);
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
                if (CameraPtr camera = layer->getCamera())
                {
                    camera->recalculateProjection();
                }
            }
        }

        void Scene::reorderLayers()
        {
            _reorderLayers = true;
        }

        void Scene::lock()
        {
            ++_locked;
        }

        void Scene::unlock()
        {
            if (--_locked == 0)
            {
                if (!_layerAddList.empty())
                {
                    for (const LayerPtr& layer : _layerAddList)
                    {
                        addLayer(layer);
                    }
                    _layerAddList.clear();
                }

                if (!_layerRemoveList.empty())
                {
                    for (const LayerPtr& layer : _layerRemoveList)
                    {
                        removeLayer(layer);
                    }
                    _layerRemoveList.clear();
                }
            }
        }

        NodePtr Scene::pickNode(const Vector2& position) const
        {
            for (std::vector<LayerPtr>::const_reverse_iterator i = _layers.rbegin(); i != _layers.rend(); ++i)
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

        std::set<NodePtr> Scene::pickNodes(const Rectangle& rectangle) const
        {
            std::set<NodePtr> result;

            for (std::vector<LayerPtr>::const_reverse_iterator i = _layers.rbegin(); i != _layers.rend(); ++i)
            {
                // TODO: use projection

                std::set<NodePtr> nodes = (*i)->pickNodes(rectangle);

                result.insert(nodes.begin(), nodes.end());
            }

            return result;
        }

    } // namespace scene
} // namespace ouzel
