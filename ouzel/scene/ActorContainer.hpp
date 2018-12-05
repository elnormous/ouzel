// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_ACTORCONTAINER_HPP
#define OUZEL_SCENE_ACTORCONTAINER_HPP

#include <memory>
#include <vector>
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace scene
    {
        class Actor;
        class Layer;

        class ActorContainer
        {
        public:
            ActorContainer();
            virtual ~ActorContainer();

            ActorContainer(const ActorContainer&) = delete;
            ActorContainer& operator=(const ActorContainer&) = delete;

            ActorContainer(ActorContainer&&) = delete;
            ActorContainer& operator=(ActorContainer&&) = delete;

            virtual void addChild(Actor* actor);
            virtual bool removeChild(Actor* actor);

            bool moveChildToBack(Actor* actor);
            bool moveChildToFront(Actor* actor);
            virtual void removeAllChildren();
            virtual bool hasChild(Actor* actor, bool recursive = false) const;
            virtual const std::vector<Actor*>& getChildren() const { return children; }

            inline Layer* getLayer() const { return layer; }

            std::vector<std::pair<Actor*, Vector3>> findActors(const Vector2& position) const;
            std::vector<Actor*> findActors(const std::vector<Vector2>& edges) const;

        protected:
            virtual void setLayer(Layer* newLayer);

            virtual void enter();
            virtual void leave();

            std::vector<Actor*> children;

            Layer* layer = nullptr;
            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_ACTORCONTAINER_HPP
