// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_ACTORCONTAINER_HPP
#define OUZEL_SCENE_ACTORCONTAINER_HPP

#include <memory>
#include <vector>
#include "math/Vector.hpp"

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

            void addChild(std::unique_ptr<Actor>&& actor);
            virtual void addChild(Actor* actor);
            virtual bool removeChild(Actor* actor);

            bool moveChildToBack(Actor* actor);
            bool moveChildToFront(Actor* actor);
            virtual void removeAllChildren();
            virtual bool hasChild(Actor* actor, bool recursive = false) const;
            virtual const std::vector<Actor*>& getChildren() const { return children; }

            inline Layer* getLayer() const { return layer; }

            std::vector<std::pair<Actor*, Vector3F>> findActors(const Vector2F& position) const;
            std::vector<Actor*> findActors(const std::vector<Vector2F>& edges) const;

        protected:
            virtual void setLayer(Layer* newLayer);

            virtual void enter();
            virtual void leave();

            std::vector<Actor*> children;
            std::vector<std::unique_ptr<Actor>> ownedChildren;

            Layer* layer = nullptr;
            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_ACTORCONTAINER_HPP
