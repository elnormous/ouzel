// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "math/Vector2.h"
#include "math/Rectangle.h"
#include "events/EventHandler.h"

namespace ouzel
{
    namespace scene
    {
        class SceneManager;

        class Scene: public Noncopyable
        {
            friend SceneManager;
        public:
            Scene();
            virtual ~Scene();

            virtual void draw();

            void addLayer(Layer* layer);
            void removeLayer(Layer* layer);
            void removeAllLayers();
            bool hasLayer(Layer* layer) const;
            const std::vector<Layer*>& getLayers() const { return layers; }

            virtual void recalculateProjection();

            Node* pickNode(const Vector2& position) const;
            std::vector<Node*> pickNodes(const Vector2& position) const;
            std::vector<Node*> pickNodes(const std::vector<Vector2>& edges) const;

        protected:
            virtual void enter();
            virtual void leave();

            bool handleWindow(Event::Type type, const WindowEvent& event);
            bool handleMouse(Event::Type type, const MouseEvent& event);
            bool handleTouch(Event::Type type, const TouchEvent& event);

            void pointerEnterNode(uint64_t pointerId, scene::Node* node, const Vector2& position);
            void pointerLeaveNode(uint64_t pointerId, scene::Node* node, const Vector2& position);
            void pointerDownOnNode(uint64_t pointerId, scene::Node* node, const Vector2& position);
            void pointerUpOnNode(uint64_t pointerId, scene::Node* node, const Vector2& position);
            void pointerDragNode(uint64_t pointerId, scene::Node* node, const Vector2& position);

            std::vector<Layer*> layers;
            ouzel::EventHandler eventHandler;

            std::map<uint64_t, scene::Node*> pointerDownOnNodes;

            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel
