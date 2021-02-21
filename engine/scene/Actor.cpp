// Ouzel by Elviss Strazdins

#include <algorithm>
#include <cassert>
#include "Actor.hpp"
#include "SceneManager.hpp"
#include "Layer.hpp"
#include "Camera.hpp"
#include "Component.hpp"
#include "../math/MathUtils.hpp"

namespace ouzel::scene
{
    ActorContainer::~ActorContainer()
    {
        for (const auto actor : children)
        {
            if (entered) actor->leave();
            actor->parent = nullptr;
            actor->setLayer(nullptr);
        }
    }

    void ActorContainer::addChild(std::unique_ptr<Actor> actor)
    {
        assert(actor.get());

        addChild(*actor);
        ownedChildren.push_back(std::move(actor));
    }

    void ActorContainer::addChild(Actor& actor)
    {
        if (actor.parent)
            actor.parent->removeChild(actor);

        actor.parent = this;
        actor.setLayer(layer);
        if (entered) actor.enter();
        children.push_back(&actor);
    }

    bool ActorContainer::removeChild(const Actor& actor)
    {
        bool result = false;

        const auto childIterator = std::find(children.begin(), children.end(), &actor);

        if (childIterator != children.end())
        {
            Actor* child = *childIterator;

            if (entered) child->leave();
            child->parent = nullptr;
            child->setLayer(nullptr);
            children.erase(childIterator);

            result = true;
        }

        const auto ownedChildIterator = std::find_if(ownedChildren.begin(), ownedChildren.end(), [&actor](const auto& ownedChild) noexcept {
            return ownedChild.get() == &actor;
        });
        if (ownedChildIterator != ownedChildren.end())
            ownedChildren.erase(ownedChildIterator);

        return result;
    }

    std::unique_ptr<Actor> ActorContainer::releaseChild(const Actor& actor)
    {
        std::unique_ptr<Actor> result;

        const auto ownedChildIterator = std::find_if(ownedChildren.begin(), ownedChildren.end(), [&actor](const auto& ownedChild) noexcept {
            return ownedChild.get() == &actor;
        });

        if (ownedChildIterator != ownedChildren.end())
        {
            result = std::move(*ownedChildIterator);
            ownedChildren.erase(ownedChildIterator);
        }

        removeChild(actor);

        return result;
    }

    bool ActorContainer::moveChildToBack(Actor& actor)
    {
        const auto i = std::find(children.begin(), children.end(), &actor);

        if (i != children.end())
        {
            std::rotate(children.begin(), i, i + 1);

            return true;
        }

        return false;
    }

    bool ActorContainer::moveChildToFront(Actor& actor)
    {
        const auto i = std::find(children.begin(), children.end(), &actor);

        if (i != children.end())
        {
            std::rotate(i, i + 1, children.end());

            return true;
        }

        return false;
    }

    void ActorContainer::removeAllChildren()
    {
        for (const auto actor : children)
        {
            if (entered) actor->leave();
            actor->parent = nullptr;
        }

        children.clear();
        ownedChildren.clear();
    }

    bool ActorContainer::hasChild(const Actor& actor, bool recursive) const
    {
        for (auto i = children.cbegin(); i != children.cend(); ++i)
        {
            const Actor* child = *i;

            if (child == &actor || (recursive && child->hasChild(actor, true)))
                return true;
        }

        return false;
    }

    void ActorContainer::enter()
    {
        entered = true;

        for (const auto actor : children)
            actor->enter();
    }

    void ActorContainer::leave()
    {
        entered = false;

        for (const auto actor : children)
            actor->leave();
    }

    void ActorContainer::setLayer(Layer* newLayer)
    {
        layer = newLayer;

        for (const auto actor : children)
            actor->setLayer(layer);
    }

    std::vector<std::pair<Actor*, Vector3F>> ActorContainer::findActors(const Vector2F& position) const
    {
        std::vector<std::pair<Actor*, Vector3F>> actors;

        std::queue<const ActorContainer*> actorContainers;
        actorContainers.push(this);

        while (!actorContainers.empty())
        {
            const auto actorContainer = actorContainers.front();
            actorContainers.pop();

            for (auto i = actorContainer->children.rbegin(); i != actorContainer->children.rend(); ++i)
            {
                Actor* actor = *i;

                if (!actor->isHidden())
                {
                    actorContainers.push(actor);

                    if (actor->isPickable() && actor->pointOn(position))
                    {
                        auto result = std::pair(actor, actor->convertWorldToLocal(Vector3F(position)));

                        const auto upperBound = std::upper_bound(actors.begin(), actors.end(), result,
                                                                 [](const auto& a, const auto& b) noexcept {
                                                                     return a.first->worldOrder < b.first->worldOrder;
                                                                 });

                        actors.insert(upperBound, result);
                    }
                }
            }
        }

        return actors;
    }

    std::vector<Actor*> ActorContainer::findActors(const std::vector<Vector2F>& edges) const
    {
        std::vector<Actor*> actors;

        std::queue<const ActorContainer*> actorContainers;
        actorContainers.push(this);

        while (!actorContainers.empty())
        {
            const auto actorContainer = actorContainers.front();
            actorContainers.pop();

            for (auto i = actorContainer->children.rbegin(); i != actorContainer->children.rend(); ++i)
            {
                Actor* actor = *i;

                if (!actor->isHidden())
                {
                    actorContainers.push(actor);

                    if (actor->isPickable() && actor->shapeOverlaps(edges))
                    {
                        const auto upperBound = std::upper_bound(actors.begin(), actors.end(), actor,
                                                                 [](const auto a, const auto b) noexcept {
                                                                     return a->worldOrder < b->worldOrder;
                                                                 });

                        actors.insert(upperBound, actor);
                    }
                }
            }
        }

        return actors;
    }

    Actor::~Actor()
    {
        if (parent) parent->removeChild(*this);

        for (const auto component : components)
            component->setActor(nullptr);
    }

    void Actor::visit(std::vector<Actor*>& drawQueue,
                      const Matrix4F& newParentTransform,
                      bool parentTransformDirty,
                      Camera* camera,
                      Order parentOrder,
                      bool parentHidden)
    {
        worldOrder = parentOrder + order;
        worldHidden = parentHidden || hidden;

        if (parentTransformDirty) updateTransform(newParentTransform);
        if (transformDirty) calculateTransform();

        if (!worldHidden)
        {
            const auto boundingBox = getBoundingBox();

            if (cullDisabled || (!boundingBox.isEmpty() && camera->checkVisibility(getTransform(), boundingBox)))
            {
                const auto upperBound = std::upper_bound(drawQueue.begin(), drawQueue.end(), this,
                                                         [](const auto a, const auto b) noexcept {
                                                             return a->worldOrder > b->worldOrder;
                                                         });

                drawQueue.insert(upperBound, this);
            }
        }

        for (const auto actor : children)
            actor->visit(drawQueue, transform, updateChildrenTransform, camera, worldOrder, worldHidden);

        updateChildrenTransform = false;
    }

    void Actor::draw(Camera* camera, bool wireframe)
    {
        if (transformDirty)
            calculateTransform();

        for (const auto component : components)
            if (!component->isHidden())
                component->draw(transform,
                                opacity,
                                camera->getRenderViewProjection(),
                                wireframe);
    }

    void Actor::addChild(Actor& actor)
    {
        ActorContainer::addChild(actor);

        actor.updateTransform(getTransform());
    }

    void Actor::setPosition(const Vector2F& newPosition)
    {
        position.v[0] = newPosition.v[0];
        position.v[1] = newPosition.v[1];

        updateLocalTransform();
    }

    void Actor::setPosition(const Vector3F& newPosition)
    {
        position = newPosition;

        updateLocalTransform();
    }

    void Actor::setRotation(const QuaternionF& newRotation)
    {
        rotation = newRotation;

        updateLocalTransform();
    }

    void Actor::setRotation(const Vector3F& newRotation)
    {
        QuaternionF roationQuaternion;
        roationQuaternion.setEulerAngles(newRotation);

        rotation = roationQuaternion;

        updateLocalTransform();
    }

    void Actor::setRotation(float newRotation)
    {
        QuaternionF roationQuaternion;
        roationQuaternion.rotate(newRotation, Vector3F{0.0F, 0.0F, 1.0F});

        rotation = roationQuaternion;

        updateLocalTransform();
    }

    void Actor::setScale(const Vector2F& newScale)
    {
        scale.v[0] = newScale.v[0];
        scale.v[1] = newScale.v[1];

        updateLocalTransform();
    }

    void Actor::setScale(const Vector3F& newScale)
    {
        scale = newScale;

        updateLocalTransform();
    }

    void Actor::setOpacity(float newOpacity)
    {
        opacity = std::clamp(newOpacity, 0.0F, 1.0F);
    }

    void Actor::setFlipX(bool newFlipX)
    {
        flipX = newFlipX;

        updateLocalTransform();
    }

    void Actor::setFlipY(bool newFlipY)
    {
        flipY = newFlipY;

        updateLocalTransform();
    }

    void Actor::setHidden(bool newHidden)
    {
        hidden = newHidden;
    }

    bool Actor::pointOn(const Vector2F& worldPosition) const
    {
        const auto localPosition = Vector2F(convertWorldToLocal(Vector3F(worldPosition)));

        for (const auto component : components)
        {
            if (component->pointOn(localPosition))
                return true;
        }

        return false;
    }

    bool Actor::shapeOverlaps(const std::vector<Vector2F>& edges) const
    {
        const auto& inverse = getInverseTransform();

        std::vector<Vector2F> transformedEdges;

        for (const auto& edge : edges)
        {
            auto transformedEdge = Vector3F(edge);

            inverse.transformPoint(transformedEdge);

            transformedEdges.emplace_back(transformedEdge.v[0], transformedEdge.v[1]);
        }

        for (const auto component : components)
        {
            if (component->shapeOverlaps(transformedEdges))
                return true;
        }

        return false;
    }

    void Actor::updateLocalTransform()
    {
        localTransformDirty = transformDirty = inverseTransformDirty = true;
        for (const auto component : components)
            component->updateTransform();
    }

    void Actor::updateTransform(const Matrix4F& newParentTransform)
    {
        parentTransform = newParentTransform;
        transformDirty = inverseTransformDirty = true;
        for (const auto component : components)
            component->updateTransform();
    }

    Vector3F Actor::getWorldPosition() const
    {
        auto result = position;
        const auto& currentTransform = getTransform();
        currentTransform.transformPoint(result);

        return position;
    }

    Vector3F Actor::convertWorldToLocal(const Vector3F& worldPosition) const
    {
        auto localPosition = worldPosition;

        const auto& currentInverseTransform = getInverseTransform();
        currentInverseTransform.transformPoint(localPosition);

        return localPosition;
    }

    Vector3F Actor::convertLocalToWorld(const Vector3F& localPosition) const
    {
        auto worldPosition = localPosition;

        const auto& currentTransform = getTransform();
        currentTransform.transformPoint(worldPosition);

        return worldPosition;
    }

    void Actor::calculateLocalTransform() const
    {
        localTransform.setTranslation(position);

        Matrix4F rotationMatrix;
        rotationMatrix.setRotation(rotation);

        localTransform *= rotationMatrix;

        const auto finalScale = Vector3F{scale.v[0] * (flipX ? -1.0F : 1.0F),
                                         scale.v[1] * (flipY ? -1.0F : 1.0F),
                                         scale.v[2]};

        Matrix4F scaleMatrix;
        scaleMatrix.setScale(finalScale);

        localTransform *= scaleMatrix;

        localTransformDirty = false;
    }

    void Actor::calculateTransform() const
    {
        transform = parentTransform * getLocalTransform();
        transformDirty = false;

        updateChildrenTransform = true;
    }

    void Actor::calculateInverseTransform() const
    {
        inverseTransform = getTransform();
        inverseTransform.invert();
        inverseTransformDirty = false;
    }

    void Actor::removeFromParent()
    {
        if (parent) parent->removeChild(*this);
    }

    void Actor::addComponent(std::unique_ptr<Component> component)
    {
        addComponent(*component);
        ownedComponents.push_back(std::move(component));
    }

    void Actor::addComponent(Component& component)
    {
        if (component.actor)
            component.actor->removeComponent(component);

        component.setActor(this);
        components.push_back(&component);
    }

    bool Actor::removeComponent(Component& component)
    {
        bool result = false;

        const auto componentIterator = std::find(components.begin(), components.end(), &component);

        if (componentIterator != components.end())
        {
            component.setActor(nullptr);
            components.erase(componentIterator);
            result = true;
        }

        const auto ownedComponentIterator = std::find_if(ownedComponents.begin(), ownedComponents.end(), [&component](const auto& ownedComponent) noexcept {
            return ownedComponent.get() == &component;
        });
        if (ownedComponentIterator != ownedComponents.end())
            ownedComponents.erase(ownedComponentIterator);

        return result;
    }

    void Actor::removeAllComponents()
    {
        for (const auto component : components)
            component->actor = nullptr;

        components.clear();
        ownedComponents.clear();
    }

    void Actor::setLayer(Layer* newLayer)
    {
        ActorContainer::setLayer(newLayer);

        for (const auto component : components)
            component->setLayer(newLayer);
    }

    Box3F Actor::getBoundingBox() const
    {
        Box3F boundingBox;

        for (const auto component : components)
            if (!component->isHidden())
                boundingBox.merge(component->getBoundingBox());

        return boundingBox;
    }
}
