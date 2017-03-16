// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include "scene/NodeContainer.h"
#include "math/Vector2.h"

namespace ouzel
{
    namespace scene
    {
        class Scene;
        class Node;
        class Camera;

        class Layer: public NodeContainer
        {
            friend Scene;
            friend Camera;
        public:
            Layer();
            virtual ~Layer();

            virtual void draw();

            void setScene(Scene* newScene);
            Scene* getScene() const { return scene; }
            void removeFromScene();

            const std::vector<Camera*>& getCameras() const { return cameras; }

            Node* pickNode(const Vector2& position) const;
            std::vector<Node*> pickNodes(const Vector2& position) const;
            std::vector<Node*> pickNodes(const std::vector<Vector2>& edges) const;

            int32_t getOrder() const { return order; }
            void setOrder(int32_t newOrder);

        protected:
            virtual void addChild(Node* node) override;
            void addCamera(Camera* camera);
            void removeCamera(Camera* camera);

            virtual void recalculateProjection();
            virtual void enter() override;

            Scene* scene = nullptr;

            std::vector<Camera*> cameras;

            int32_t order = 0;
        };
    } // namespace scene
} // namespace ouzel
