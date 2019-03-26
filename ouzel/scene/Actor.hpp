// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_ACTOR_HPP
#define OUZEL_SCENE_ACTOR_HPP

#include "scene/ActorContainer.hpp"
#include "math/Box.hpp"
#include "math/Color.hpp"
#include "math/Matrix.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    namespace scene
    {
        class Camera;
        class Component;
        class Layer;

        class Actor: public ActorContainer
        {
            friend ActorContainer;
            friend Layer;
        public:
            Actor();
            virtual ~Actor();

            virtual void visit(std::vector<Actor*>& drawQueue,
                               const Matrix4F& newParentTransform,
                               bool parentTransformDirty,
                               Camera* camera,
                               int32_t parentOrder,
                               bool parentHidden);
            virtual void draw(Camera* camera, bool wireframe);

            virtual const Vector3F& getPosition() const { return position; }
            virtual void setPosition(const Vector2F& newPosition);
            virtual void setPosition(const Vector3F& newPosition);

            inline int32_t getOrder() const { return order; }
            inline void setOrder(int32_t newOrder) { order = newOrder; }

            virtual const QuaternionF& getRotation() const { return rotation; }
            virtual void setRotation(const QuaternionF& newRotation);
            virtual void setRotation(const Vector3F& newRotation);
            virtual void setRotation(float newRotation);

            virtual const Vector3F& getScale() const { return scale; }
            virtual void setScale(const Vector2F& newScale);
            virtual void setScale(const Vector3F& newScale);

            virtual float getOpacity() const { return opacity; }
            virtual void setOpacity(float newOpacity);

            virtual bool getFlipX() const { return flipX; }
            virtual void setFlipX(bool newFlipX);

            virtual bool getFlipY() const { return flipY; }
            virtual void setFlipY(bool newFlipY);

            virtual bool isPickable() const { return pickable; }
            virtual void setPickable(bool newPickable) { pickable = newPickable; }

            virtual bool isCullDisabled() const { return cullDisabled; }
            virtual void setCullDisabled(bool newCullDisabled) { cullDisabled = newCullDisabled; }

            virtual bool isHidden() const { return hidden; }
            virtual void setHidden(bool newHidden);
            inline bool isWorldHidden() const { return worldHidden; }

            virtual bool pointOn(const Vector2F& worldPosition) const;
            virtual bool shapeOverlaps(const std::vector<Vector2F>& edges) const;

            const Matrix4F& getLocalTransform() const
            {
                if (localTransformDirty) calculateLocalTransform();

                return localTransform;
            }

            const Matrix4F& getTransform() const
            {
                if (transformDirty) calculateTransform();

                return transform;
            }

            const Matrix4F& getInverseTransform() const
            {
                if (inverseTransformDirty) calculateInverseTransform();

                return inverseTransform;
            }

            Vector3F getWorldPosition() const;
            virtual int32_t getWorldOrder() const { return worldOrder; }

            Vector3F convertWorldToLocal(const Vector3F& worldPosition) const;
            Vector3F convertLocalToWorld(const Vector3F& localPosition) const;

            inline ActorContainer* getParent() const { return parent; }
            void removeFromParent();

            void addChild(Actor* actor) override;

            void addComponent(std::unique_ptr<Component>&& component);
            void addComponent(Component* component);
            bool removeComponent(Component* component);

            void removeAllComponents();
            inline const std::vector<Component*>& getComponents() const { return components; }
            std::vector<Component*> getComponents(uint32_t cls) const;

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
            Vector3F scale = Vector3F(1.0F, 1.0F, 1.0F);
            float opacity = 1.0F;
            int32_t order = 0;
            int32_t worldOrder = 0;

            ActorContainer* parent = nullptr;

            std::vector<Component*> components;
            std::vector<std::unique_ptr<Component>> ownedComponents;

            EventHandler animationUpdateHandler;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_ACTOR_HPP
