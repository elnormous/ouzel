// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include "utils/Noncopyable.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace scene
    {
        class Actor;
        class Layer;

        class ActorContainer: public Noncopyable
        {
        public:
            ActorContainer();
            virtual ~ActorContainer();

            void addChild(Actor* actor)
            {
                addChildActor(actor);
            }

            template<class T> void addChild(const std::unique_ptr<T>& actor)
            {
                addChildActor(actor.get());
            }

            template<class T> void addChild(std::unique_ptr<T>&& actor)
            {
                addChildActor(actor.get());
                ownedChildren.push_back(std::move(actor));
            }

            template<class T> bool removeChild(const std::unique_ptr<T>& actor)
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

            Layer* getLayer() const { return layer; }

        protected:
            virtual void addChildActor(Actor* actor);
            virtual bool removeChildActor(Actor* actor);

            virtual void setLayer(Layer* newLayer);
            virtual void findActors(const Vector2& position, std::vector<std::pair<Actor*, ouzel::Vector3>>& actors) const;
            virtual void findActors(const std::vector<Vector2>& edges, std::vector<Actor*>& actors) const;

            virtual void enter();
            virtual void leave();

            std::vector<Actor*> children;
            std::vector<std::unique_ptr<Actor>> ownedChildren;

            Layer* layer = nullptr;
            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel
