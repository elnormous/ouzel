// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <set>
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
        class Camera;
        class Scene;

        class Layer: public NodeContainer
        {
            friend Scene;
        public:
            Layer();
            virtual ~Layer();

            virtual void draw();

            virtual bool addChild(const NodePtr& node) override;

            void addToDrawQueue(const NodePtr& node);

            const CameraPtr& getCamera() const { return camera; }
            void setCamera(const CameraPtr& newCamera);

            NodePtr pickNode(const Vector2& position) const;
            std::set<NodePtr> pickNodes(const std::vector<Vector2>& edges) const;

            int32_t getOrder() const { return order; }
            void setOrder(int32_t newOrder);

            void setRenderTarget(const graphics::RenderTargetPtr& newRenderTarget);
            const graphics::RenderTargetPtr& getRenderTarget() const { return renderTarget; }

            bool checkVisibility(const NodePtr& node) const;

        protected:
            CameraPtr camera;
            std::vector<NodePtr> drawQueue;

            int32_t order = 0;

            graphics::RenderTargetPtr renderTarget;
        };
    } // namespace scene
} // namespace ouzel
