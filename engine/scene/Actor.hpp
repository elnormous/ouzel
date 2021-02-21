// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_ACTOR_HPP
#define OUZEL_SCENE_ACTOR_HPP

#include <memory>
#include <vector>
#include "../math/Box.hpp"
#include "../math/Color.hpp"
#include "../math/Matrix.hpp"
#include "../math/Quaternion.hpp"
#include "../math/Vector.hpp"
#include "../events/EventHandler.hpp"

namespace ouzel::scene
{
    class Actor;
    class Camera;
    class Component;
    class Layer;

    class ActorContainer
    {
    public:
        ActorContainer() = default;
        virtual ~ActorContainer();

        ActorContainer(const ActorContainer&) = delete;
        ActorContainer& operator=(const ActorContainer&) = delete;

        ActorContainer(ActorContainer&&) = delete;
        ActorContainer& operator=(ActorContainer&&) = delete;

        void addChild(std::unique_ptr<Actor> actor);
        virtual void addChild(Actor& actor);
        virtual bool removeChild(const Actor& actor);
        std::unique_ptr<Actor> releaseChild(const Actor& actor);

        bool moveChildToBack(Actor& actor);
        bool moveChildToFront(Actor& actor);
        virtual void removeAllChildren();
        virtual bool hasChild(const Actor& actor, bool recursive = false) const;
        virtual const std::vector<Actor*>& getChildren() const noexcept { return children; }

        auto getLayer() const noexcept { return layer; }

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

    class Actor: public ActorContainer
    {
        friend ActorContainer;
        friend Layer;
    public:
        using Order = std::int32_t;

        Actor() = default;
        ~Actor() override;

        virtual void visit(std::vector<Actor*>& drawQueue,
                           const Matrix4F& newParentTransform,
                           bool parentTransformDirty,
                           Camera* camera,
                           Order parentOrder,
                           bool parentHidden);
        virtual void draw(Camera* camera, bool wireframe);

        virtual const Vector3F& getPosition() const noexcept { return position; }
        virtual void setPosition(const Vector2F& newPosition);
        virtual void setPosition(const Vector3F& newPosition);

        auto getOrder() const noexcept { return order; }
        void setOrder(Order newOrder) { order = newOrder; }

        virtual const QuaternionF& getRotation() const noexcept { return rotation; }
        virtual void setRotation(const QuaternionF& newRotation);
        virtual void setRotation(const Vector3F& newRotation);
        virtual void setRotation(float newRotation);

        virtual const Vector3F& getScale() const noexcept { return scale; }
        virtual void setScale(const Vector2F& newScale);
        virtual void setScale(const Vector3F& newScale);

        virtual float getOpacity() const noexcept { return opacity; }
        virtual void setOpacity(float newOpacity);

        virtual bool getFlipX() const noexcept { return flipX; }
        virtual void setFlipX(bool newFlipX);

        virtual bool getFlipY() const noexcept { return flipY; }
        virtual void setFlipY(bool newFlipY);

        virtual bool isPickable() const noexcept { return pickable; }
        virtual void setPickable(bool newPickable) { pickable = newPickable; }

        virtual bool isCullDisabled() const noexcept { return cullDisabled; }
        virtual void setCullDisabled(bool newCullDisabled) { cullDisabled = newCullDisabled; }

        virtual bool isHidden() const noexcept { return hidden; }
        virtual void setHidden(bool newHidden);
        auto isWorldHidden() const noexcept { return worldHidden; }

        virtual bool pointOn(const Vector2F& worldPosition) const;
        virtual bool shapeOverlaps(const std::vector<Vector2F>& edges) const;

        auto& getLocalTransform() const
        {
            if (localTransformDirty) calculateLocalTransform();

            return localTransform;
        }

        auto& getTransform() const
        {
            if (transformDirty) calculateTransform();

            return transform;
        }

        auto& getInverseTransform() const
        {
            if (inverseTransformDirty) calculateInverseTransform();

            return inverseTransform;
        }

        Vector3F getWorldPosition() const;
        virtual std::int32_t getWorldOrder() const { return worldOrder; }

        Vector3F convertWorldToLocal(const Vector3F& worldPosition) const;
        Vector3F convertLocalToWorld(const Vector3F& localPosition) const;

        auto getParent() const noexcept { return parent; }
        void removeFromParent();

        void addChild(Actor& actor) override;

        void addComponent(std::unique_ptr<Component> component);
        void addComponent(Component& component);
        bool removeComponent(Component& component);

        void removeAllComponents();
        auto& getComponents() const noexcept { return components; }

        Box3F getBoundingBox() const;

    protected:
        void setLayer(Layer* newLayer) override;

        void updateLocalTransform();
        void updateTransform(const Matrix4F& newParentTransform);

        virtual void calculateLocalTransform() const;
        virtual void calculateTransform() const;

        virtual void calculateInverseTransform() const;

        Matrix4F parentTransform;
        mutable Matrix4F transform;
        mutable Matrix4F inverseTransform;
        mutable Matrix4F localTransform;

        mutable bool transformDirty = true;
        mutable bool inverseTransformDirty = true;
        mutable bool localTransformDirty = true;
        mutable bool updateChildrenTransform = true;

        bool flipX = false;
        bool flipY = false;

        bool pickable = false;
        bool cullDisabled = false;
        bool hidden = false;
        bool worldHidden = false;

        Vector3F position;
        QuaternionF rotation = QuaternionF::identity();
        Vector3F scale = Vector3F{1.0F, 1.0F, 1.0F};
        float opacity = 1.0F;
        Order order = 0;
        Order worldOrder = 0;

        ActorContainer* parent = nullptr;

        std::vector<Component*> components;
        std::vector<std::unique_ptr<Component>> ownedComponents;

        EventHandler animationUpdateHandler;
    };
}

#endif // OUZEL_SCENE_ACTOR_HPP
