// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "math/Vector2.hpp"
#include "utils/Memory.hpp"

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

            void addChild(Actor* actor)
            {
                addChildActor(actor);
            }

            template<class T> void addChild(const UniquePtr<T>& actor)
            {
                addChildActor(actor.get());
            }

            template<class T> void addChild(UniquePtr<T>&& actor)
            {
                addChildActor(actor.get());
                ownedChildren.push_back(std::move(actor));
            }

            template<class T> bool removeChild(const UniquePtr<T>& actor)
            {
                return removeChildActor(actor.get());
            }

            bool removeChild(Actor* actor)
            {
                return removeChildActor(actor);
            }

            bool moveChildToBack(Actor* actor);
            bool moveChildToFront(Actor* actor);
            virtual void removeAllChildren();
            virtual bool hasChild(Actor* actor, bool recursive = false) const;
            virtual const std::vector<Actor*>& getChildren() const { return children; }

            inline Layer* getLayer() const { return layer; }

            virtual void findActors(const Vector2& position, std::vector<std::pair<Actor*, Vector3>>& actors) const;
            virtual void findActors(const std::vector<Vector2>& edges, std::vector<Actor*>& actors) const;

        protected:
            virtual void addChildActor(Actor* actor);
            virtual bool removeChildActor(Actor* actor);

            virtual void setLayer(Layer* newLayer);

            virtual void enter();
            virtual void leave();

            std::vector<Actor*> children;
            std::vector<UniquePtr<Actor>> ownedChildren;

            Layer* layer = nullptr;
            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel
