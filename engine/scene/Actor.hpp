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

        std::vector<std::pair<Actor*, math::Vector<float, 3>>> findActors(const math::Vector<float, 2>& position) const;
        std::vector<Actor*> findActors(const std::vector<math::Vector<float, 2>>& edges) const;

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
                           const math::Matrix<float, 4>& newParentTransform,
                           bool parentTransformDirty,
                           Camera* camera,
                           Order parentOrder,
                           bool parentHidden);
        virtual void draw(Camera* camera, bool wireframe);

        virtual const math::Vector<float, 3>& getPosition() const noexcept { return position; }
        virtual void setPosition(const math::Vector<float, 2>& newPosition);
        virtual void setPosition(const math::Vector<float, 3>& newPosition);

        auto getOrder() const noexcept { return order; }
        void setOrder(Order newOrder) noexcept { order = newOrder; }

        virtual const math::Quaternion<float>& getRotation() const noexcept { return rotation; }
        virtual void setRotation(const math::Quaternion<float>& newRotation);
        virtual void setRotation(const math::Vector<float, 3>& newRotation);
        virtual void setRotation(float newRotation);

        virtual const math::Vector<float, 3>& getScale() const noexcept { return scale; }
        virtual void setScale(const math::Vector<float, 2>& newScale);
        virtual void setScale(const math::Vector<float, 3>& newScale);

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

        virtual bool pointOn(const math::Vector<float, 2>& worldPosition) const;
        virtual bool shapeOverlaps(const std::vector<math::Vector<float, 2>>& edges) const;

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

        math::Vector<float, 3> getWorldPosition() const;
        virtual Order getWorldOrder() const { return worldOrder; }

        math::Vector<float, 3> convertWorldToLocal(const math::Vector<float, 3>& worldPosition) const;
        math::Vector<float, 3> convertLocalToWorld(const math::Vector<float, 3>& localPosition) const;

        auto getParent() const noexcept { return parent; }
        void removeFromParent();

        void addChild(Actor& actor) override;

        void addComponent(std::unique_ptr<Component> component);
        void addComponent(Component& component);
        bool removeComponent(Component& component);

        void removeAllComponents();
        auto& getComponents() const noexcept { return components; }

        math::Box<float, 3> getBoundingBox() const;

    protected:
        void setLayer(Layer* newLayer) override;

        void updateLocalTransform();
        void updateTransform(const math::Matrix<float, 4>& newParentTransform);

        virtual void calculateLocalTransform() const;
        virtual void calculateTransform() const;

        virtual void calculateInverseTransform() const;

        math::Matrix<float, 4> parentTransform = math::identityMatrix<float, 4>;
        mutable math::Matrix<float, 4> transform;
        mutable math::Matrix<float, 4> inverseTransform;
        mutable math::Matrix<float, 4> localTransform;

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

        math::Vector<float, 3> position{};
        math::Quaternion<float> rotation = math::identityQuaternion<float>;
        math::Vector<float, 3> scale = math::Vector<float, 3>{1.0F, 1.0F, 1.0F};
        float opacity = 1.0F;
        Order order = 0;
        Order worldOrder = 0;

        ActorContainer* parent = nullptr;

        std::vector<Component*> components;
        std::vector<std::unique_ptr<Component>> ownedComponents;
    };
}

#endif // OUZEL_SCENE_ACTOR_HPP
