// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef ACTORCONTAINER_HPP
#define ACTORCONTAINER_HPP

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

            template<typename T> void addChild(const std::unique_ptr<T>& actor)
            {
                addChild(actor.get());
            }

            template<typename T> void addChild(std::unique_ptr<T>&& actor)
            {
                addChild(actor.get());
                ownedChildren.push_back(std::move(actor));
            }

            virtual bool removeChild(Actor* actor);

            template<typename T> bool removeChild(const std::unique_ptr<T>& actor)
            {
                return removeChild(actor.get());
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

#endif // ACTORCONTAINER_HPP
