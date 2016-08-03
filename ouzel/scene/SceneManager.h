// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <map>
#include <cstdint>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "math/Vector2.h"
#include "math/Rectangle.h"
#include "events/EventHandler.h"

namespace ouzel
{
    class Engine;

    namespace scene
    {
        class SceneManager: public Noncopyable
        {
            friend Engine;
        public:
            virtual ~SceneManager();

            void draw();

            void setScene(const ScenePtr& newScene);
            const ScenePtr& getScene() const { return scene; }

            virtual void recalculateProjection();

        protected:
            SceneManager();
            bool handleMouse(Event::Type type, const MouseEvent& event);
            bool handleTouch(Event::Type type, const TouchEvent& event);

            scene::NodePtr getPointerOnNode(uint64_t pointerId) const;
            scene::NodePtr getPointerDownOnNode(uint64_t pointerId) const;

            void pointerEnterNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position);
            void pointerLeaveNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position);
            void pointerDownOnNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position);
            void pointerUpOnNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position);
            void pointerDragNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position);

            ScenePtr scene;
            ouzel::EventHandler eventHandler;

            std::map<uint64_t, scene::NodeWeakPtr> pointerOnNodes;
            std::map<uint64_t, scene::NodeWeakPtr> pointerDownOnNodes;
        };
    } // namespace scene
} // namespace ouzel
