// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include "utils/Noncopyable.h"
#include "math/Vector2.h"
#include "events/EventHandler.h"

namespace ouzel
{
    namespace scene
    {
        class SceneManager;
        class Layer;

        class Scene: public Noncopyable
        {
            friend SceneManager;
        public:
            Scene();
            virtual ~Scene();

            virtual void draw();

            void addLayer(const std::shared_ptr<Layer>& layer);
            bool removeLayer(const std::shared_ptr<Layer>& layer);
            void removeAllLayers();
            bool hasLayer(const std::shared_ptr<Layer>& layer) const;
            const std::vector<std::shared_ptr<Layer>>& getLayers() const { return layers; }

            virtual void recalculateProjection();

            std::shared_ptr<Node> pickNode(const Vector2& position) const;
            std::vector<std::shared_ptr<Node>> pickNodes(const Vector2& position) const;
            std::vector<std::shared_ptr<Node>> pickNodes(const std::vector<Vector2>& edges) const;

        protected:
            virtual void enter();
            virtual void leave();

            bool handleWindow(Event::Type type, const WindowEvent& event);
            bool handleMouse(Event::Type type, const MouseEvent& event);
            bool handleTouch(Event::Type type, const TouchEvent& event);

            void pointerEnterNode(uint64_t pointerId, const std::shared_ptr<Node>& node, const Vector2& position);
            void pointerLeaveNode(uint64_t pointerId, const std::shared_ptr<Node>& node, const Vector2& position);
            void pointerDownOnNode(uint64_t pointerId, const std::shared_ptr<Node>& node, const Vector2& position);
            void pointerUpOnNode(uint64_t pointerId, const std::shared_ptr<Node>& node, const Vector2& position);
            void pointerDragNode(uint64_t pointerId, const std::shared_ptr<Node>& node, const Vector2& position, const Vector2& difference);

            std::vector<std::shared_ptr<Layer>> layers;
            ouzel::EventHandler eventHandler;

            std::unordered_map<uint64_t, std::shared_ptr<Node>> pointerDownOnNodes;

            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel
