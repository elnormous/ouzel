// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "utils/Types.h"
#include "scene/NodeContainer.h"
#include "math/Size2.h"
#include "math/Matrix4.h"
#include "math/Vector2.h"
#include "math/Rectangle.h"

namespace ouzel
{
    namespace scene
    {
        class Scene;

        class Layer: public NodeContainer
        {
            friend Scene;
        public:
            Layer();
            virtual ~Layer();

            virtual void draw();

            virtual void addChild(Node* node) override;

            void addCamera(Camera* camera);
            void removeCamera(Camera* camera);
            const std::vector<Camera*>& getCameras() const { return cameras; }

            Node* pickNode(const Vector2& position) const;
            std::vector<Node*> pickNodes(const Vector2& position) const;
            std::vector<Node*> pickNodes(const std::vector<Vector2>& edges) const;

            int32_t getOrder() const { return order; }
            void setOrder(int32_t newOrder);

        protected:
            virtual void recalculateProjection();
            virtual void enter() override;

            Scene* scene = nullptr;

            std::vector<Camera*> cameras;

            int32_t order = 0;
        };
    } // namespace scene
} // namespace ouzel
