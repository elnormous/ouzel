// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Engine.h"
#include "Layer.h"
#include "Node.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4.h"

namespace ouzel
{
    namespace scene
    {
        std::shared_ptr<Layer> Layer::create()
        {
            std::shared_ptr<Layer> result = std::make_shared<Layer>();

            if (!result->init())
            {
                result.reset();
            }

            return result;
        }

        Layer::Layer()
        {

        }

        Layer::~Layer()
        {

        }

        bool Layer::init()
        {
            _camera = std::make_shared<Camera>();
            _camera->addToLayer(std::static_pointer_cast<Layer>(shared_from_this()));

            return true;
        }

        void Layer::draw()
        {
            _drawQueue.clear();

            // render only if there is an active camera
            if (_camera)
            {
                lock();

                for (const NodePtr child : _children)
                {
                    child->visit(Matrix4::IDENTITY, false);
                }

                std::stable_sort(_drawQueue.begin(), _drawQueue.end(), [](const NodePtr& a, const NodePtr& b) {
                    return a->getZ() > b->getZ();
                });

                for (const NodePtr& node : _drawQueue)
                {
                    node->process();
                }

                unlock();
            }
        }

        bool Layer::addChild(const NodePtr& node)
        {
            if (NodeContainer::addChild(node))
            {
                node->addToLayer(std::static_pointer_cast<Layer>(shared_from_this()));

                node->updateTransform(Matrix4::IDENTITY);

                return true;
            }
            else
            {
                return false;
            }
        }

        void Layer::addToDrawQueue(const NodePtr& node)
        {
            _drawQueue.push_back(node);
        }

        void Layer::setCamera(const CameraPtr& camera)
        {
            _camera = camera;
        }

        NodePtr Layer::pickNode(const Vector2& position)
        {
            for (std::vector<NodePtr>::const_reverse_iterator i = _drawQueue.rbegin(); i != _drawQueue.rend(); ++i)
            {
                NodePtr node = *i;

                if (node->isPickable() && node->pointOn(position))
                {
                    return node;
                }
            }

            return nullptr;
        }

        std::set<NodePtr> Layer::pickNodes(const Rectangle& rectangle)
        {
            std::set<NodePtr> result;

            for (std::vector<NodePtr>::const_reverse_iterator i = _drawQueue.rbegin(); i != _drawQueue.rend(); ++i)
            {
                NodePtr node = *i;

                if (node->isPickable() && node->rectangleOverlaps(rectangle))
                {
                    result.insert(node);
                }
            }

            return result;
        }

        void Layer::setOrder(int32_t order)
        {
            _order = order;

            if (ScenePtr scene = _scene.lock())
            {
                scene->reorderLayers();
            }
        }

        void Layer::addToScene(const ScenePtr& scene)
        {
            _scene = scene;
        }

        void Layer::removeFromScene()
        {
            _scene.reset();
        }
    } // namespace scene
} // namespace ouzel
