// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_ACTOR_HPP
#define OUZEL_SCENE_ACTOR_HPP

#include "scene/ActorContainer.hpp"
#include "math/Box3.hpp"
#include "math/Color.hpp"
#include "math/Matrix4.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
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
                               const Matrix4<float>& newParentTransform,
                               bool parentTransformDirty,
                               Camera* camera,
                               int32_t parentOrder,
                               bool parentHidden);
            virtual void draw(Camera* camera, bool wireframe);

            virtual const Vector3<float>& getPosition() const { return position; }
            virtual void setPosition(const Vector2<float>& newPosition);
            virtual void setPosition(const Vector3<float>& newPosition);

            inline int32_t getOrder() const { return order; }
            void setOrder(int32_t newOrder) { order = newOrder; }

            virtual const Quaternion<float>& getRotation() const { return rotation; }
            virtual void setRotation(const Quaternion<float>& newRotation);
            virtual void setRotation(const Vector3<float>& newRotation);
            virtual void setRotation(float newRotation);

            virtual const Vector3<float>& getScale() const { return scale; }
            virtual void setScale(const Vector2<float>& newScale);
            virtual void setScale(const Vector3<float>& newScale);

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
            bool isWorldHidden() const { return worldHidden; }

            virtual bool pointOn(const Vector2<float>& worldPosition) const;
            virtual bool shapeOverlaps(const std::vector<Vector2<float>>& edges) const;

            const Matrix4<float>& getLocalTransform() const
            {
                if (localTransformDirty) calculateLocalTransform();

                return localTransform;
            }

            const Matrix4<float>& getTransform() const
            {
                if (transformDirty) calculateTransform();

                return transform;
            }

            const Matrix4<float>& getInverseTransform() const
            {
                if (inverseTransformDirty) calculateInverseTransform();

                return inverseTransform;
            }

            Vector3<float> getWorldPosition() const;
            virtual int32_t getWorldOrder() const { return worldOrder; }

            Vector3<float> convertWorldToLocal(const Vector3<float>& worldPosition) const;
            Vector3<float> convertLocalToWorld(const Vector3<float>& localPosition) const;

            inline ActorContainer* getParent() const { return parent; }
            void removeFromParent();

            void addChild(Actor* actor) override;

            void addComponent(std::unique_ptr<Component>&& component);
            void addComponent(Component* component);
            bool removeComponent(Component* component);

            void removeAllComponents();
            inline const std::vector<Component*>& getComponents() const { return components; }
            std::vector<Component*> getComponents(uint32_t cls) const;

            Box3<float> getBoundingBox() const;

        protected:
            void setLayer(Layer* newLayer) override;

            void updateLocalTransform();
            void updateTransform(const Matrix4<float>& newParentTransform);

            virtual void calculateLocalTransform() const;
            virtual void calculateTransform() const;

            virtual void calculateInverseTransform() const;

            Matrix4<float> parentTransform;
            mutable Matrix4<float> transform;
            mutable Matrix4<float> inverseTransform;
            mutable Matrix4<float> localTransform;

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

            Vector3<float> position;
            Quaternion<float> rotation = Quaternion<float>::identity();
            Vector3<float> scale = Vector3<float>(1.0F, 1.0F, 1.0F);
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
