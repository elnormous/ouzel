// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cassert>
#include <algorithm>
#include <stack>
#include "ActorContainer.hpp"
#include "Actor.hpp"

namespace ouzel
{
    namespace scene
    {
        ActorContainer::ActorContainer()
        {
        }

        ActorContainer::~ActorContainer()
        {
            for (auto& actor : children)
            {
                if (entered) actor->leave();
                actor->parent = nullptr;
                actor->setLayer(nullptr);
            }
        }

        void ActorContainer::addChild(Actor* actor)
        {
            assert(actor);

            if (actor->parent)
                actor->parent->removeChild(actor);

            actor->parent = this;
            actor->setLayer(layer);
            if (entered) actor->enter();
            children.push_back(actor);
        }

        bool ActorContainer::removeChild(Actor* actor)
        {
            bool result = false;

            auto childIterator = std::find(children.begin(), children.end(), actor);

            if (childIterator != children.end())
            {
                if (entered) actor->leave();
                actor->parent = nullptr;
                actor->setLayer(nullptr);
                children.erase(childIterator);

                result = true;
            }

            return result;
        }

        bool ActorContainer::moveChildToBack(Actor* actor)
        {
            auto i = std::find(children.begin(), children.end(), actor);

            if (i != children.end())
            {
                std::rotate(children.begin(), i, i + 1);

                return true;
            }

            return false;
        }

        bool ActorContainer::moveChildToFront(Actor* actor)
        {
            auto i = std::find(children.begin(), children.end(), actor);

            if (i != children.end())
            {
                std::rotate(i, i + 1, children.end());

                return true;
            }

            return false;
        }

        void ActorContainer::removeAllChildren()
        {
            for (auto& actor : children)
            {
                if (entered) actor->leave();
                actor->parent = nullptr;
            }

            children.clear();
        }

        bool ActorContainer::hasChild(Actor* actor, bool recursive) const
        {
            for (auto i = children.cbegin(); i != children.cend(); ++i)
            {
                Actor* child = *i;

                if (child == actor || (recursive && child->hasChild(actor, true)))
                    return true;
            }

            return false;
        }

        void ActorContainer::enter()
        {
            entered = true;

            for (Actor* actor : children)
                actor->enter();
        }

        void ActorContainer::leave()
        {
            entered = false;

            for (Actor* actor : children)
                actor->leave();
        }

        void ActorContainer::setLayer(Layer* newLayer)
        {
            layer = newLayer;

            for (Actor* actor : children)
                actor->setLayer(layer);
        }

        std::vector<std::pair<Actor*, Vector3>> ActorContainer::findActors(const Vector2& position) const
        {
            std::vector<std::pair<Actor*, Vector3>> actors;

            std::stack<const ActorContainer*> actorContainers;
            actorContainers.push(this);

            while (!actorContainers.empty())
            {
                const ActorContainer* actorContainer = actorContainers.top();
                actorContainers.pop();

                for (auto i = actorContainer->children.rbegin(); i != actorContainer->children.rend(); ++i)
                {
                    Actor* actor = *i;

                    if (!actor->isHidden())
                    {
                        actorContainers.push(actor);

                        if (actor->isPickable() && actor->pointOn(position))
                        {
                            std::pair<Actor*, Vector3> result = std::make_pair(actor, actor->convertWorldToLocal(Vector3(position)));

                            auto upperBound = std::upper_bound(actors.begin(), actors.end(), result,
                                                               [](const std::pair<Actor*, Vector3>& a,
                                                                  const std::pair<Actor*, Vector3>& b) {
                                                                   return a.first->worldOrder < b.first->worldOrder;
                                                               });

                            actors.insert(upperBound, result);
                        }
                    }
                }
            }

            return actors;
        }

        std::vector<Actor*> ActorContainer::findActors(const std::vector<Vector2>& edges) const
        {
            std::vector<Actor*> actors;

            std::stack<const ActorContainer*> actorContainers;
            actorContainers.push(this);

            while (!actorContainers.empty())
            {
                const ActorContainer* actorContainer = actorContainers.top();
                actorContainers.pop();

                for (auto i = actorContainer->children.rbegin(); i != actorContainer->children.rend(); ++i)
                {
                    Actor* actor = *i;

                    if (!actor->isHidden())
                    {
                        actorContainers.push(actor);

                        if (actor->isPickable() && actor->shapeOverlaps(edges))
                        {
                            auto upperBound = std::upper_bound(actors.begin(), actors.end(), actor,
                                                               [](Actor* a, Actor* b) {
                                                                   return a->worldOrder < b->worldOrder;
                                                               });

                            actors.insert(upperBound, actor);
                        }
                    }
                }
            }

            return actors;
        }
    } // namespace scene
} // namespace ouzel
