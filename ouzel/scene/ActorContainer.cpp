// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cassert>
#include <algorithm>
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
            {
                actor->parent->removeChild(actor);
            }

            actor->parent = this;
            actor->setLayer(layer);
            if (entered) actor->enter();
            children.push_back(actor);
        }

        bool ActorContainer::removeChild(Actor* actor)
        {
            bool result = false;

            std::vector<Actor*>::iterator childIterator = std::find(children.begin(), children.end(), actor);

            if (childIterator != children.end())
            {
                if (entered) actor->leave();
                actor->parent = nullptr;
                actor->setLayer(nullptr);
                children.erase(childIterator);

                result = true;
            }

            std::vector<std::unique_ptr<Actor>>::iterator ownedIterator = std::find_if(ownedChildren.begin(), ownedChildren.end(), [actor](const std::unique_ptr<Actor>& other) {
                return other.get() == actor;
            });

            if (ownedIterator != ownedChildren.end())
            {
                ownedChildren.erase(ownedIterator);
            }

            return result;
        }

        bool ActorContainer::moveChildToBack(Actor* actor)
        {
            std::vector<Actor*>::iterator i = std::find(children.begin(), children.end(), actor);

            if (i != children.end())
            {
                std::rotate(children.begin(), i, i + 1);

                return true;
            }

            return false;
        }

        bool ActorContainer::moveChildToFront(Actor* actor)
        {
            std::vector<Actor*>::iterator i = std::find(children.begin(), children.end(), actor);

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
            ownedChildren.clear();
        }

        bool ActorContainer::hasChild(Actor* actor, bool recursive) const
        {
            for (std::vector<Actor*>::const_iterator i = children.begin(); i != children.end(); ++i)
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
            {
                actor->enter();
            }
        }

        void ActorContainer::leave()
        {
            entered = false;

            for (Actor* actor : children)
            {
                actor->leave();
            }
        }

        void ActorContainer::setLayer(Layer* newLayer)
        {
            layer = newLayer;

            for (Actor* actor : children)
            {
                actor->setLayer(layer);
            }
        }

        void ActorContainer::findActors(const Vector2& position, std::vector<std::pair<Actor*, Vector3>>& actors) const
        {
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                Actor* actor = *i;

                if (!actor->isHidden())
                {
                    actor->findActors(position, actors);

                    if (actor->isPickable() && actor->pointOn(position))
                    {
                        std::pair<Actor*, ouzel::Vector3> result = std::make_pair(actor, actor->convertWorldToLocal(position));

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

        void ActorContainer::findActors(const std::vector<Vector2>& edges, std::vector<Actor*>& actors) const
        {
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                Actor* actor = *i;

                if (!actor->isHidden())
                {
                    actor->findActors(edges, actors);

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
    } // namespace scene
} // namespace ouzel
