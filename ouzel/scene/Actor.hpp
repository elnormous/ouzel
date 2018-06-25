// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include "scene/ActorContainer.hpp"
#include "math/Box3.hpp"
#include "math/Color.hpp"
#include "math/Matrix4.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "scene/UpdateCallback.hpp"

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
                               const Matrix4& newParentTransform,
                               bool parentTransformDirty,
                               Camera* camera,
                               int32_t parentOrder,
                               bool parentHidden);
            virtual void draw(Camera* camera, bool wireframe);

            virtual void setPosition(const Vector2& newPosition);
            virtual void setPosition(const Vector3& newPosition);
            virtual const Vector3& getPosition() const { return position; }

            void setOrder(int32_t newOrder) { order = newOrder; }
            inline int32_t getOrder() const { return order; }

            virtual void setRotation(const Quaternion& newRotation);
            virtual void setRotation(const Vector3& newRotation);
            virtual void setRotation(float newRotation);
            virtual const Quaternion& getRotation() const { return rotation; }

            virtual void setScale(const Vector2& newScale);
            virtual void setScale(const Vector3& newScale);
            virtual const Vector3& getScale() const { return scale; }

            virtual void setOpacity(float newOpacity);
            virtual float getOpacity() const { return opacity; }

            virtual void setFlipX(bool newFlipX);
            virtual bool getFlipX() const { return flipX; }

            virtual void setFlipY(bool newFlipY);
            virtual bool getFlipY() const { return flipY; }

            virtual void setPickable(bool newPickable) { pickable = newPickable; }
            virtual bool isPickable() const { return pickable; }

            virtual bool isCullDisabled() const { return cullDisabled; }
            virtual void setCullDisabled(bool newCullDisabled) { cullDisabled = newCullDisabled; }

            virtual void setHidden(bool newHidden);
            virtual bool isHidden() const { return hidden; }
            bool isWorldHidden() const { return worldHidden; }

            virtual bool pointOn(const Vector2& worldPosition) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

            const Matrix4& getLocalTransform() const
            {
                if (localTransformDirty) calculateLocalTransform();

                return localTransform;
            }

            const Matrix4& getTransform() const
            {
                if (transformDirty) calculateTransform();

                return transform;
            }

            const Matrix4& getInverseTransform() const
            {
                if (inverseTransformDirty) calculateInverseTransform();

                return inverseTransform;
            }

            Vector3 getWorldPosition() const;
            virtual int32_t getWorldOrder() const { return worldOrder; }

            Vector3 convertWorldToLocal(const Vector3& worldPosition) const;
            Vector3 convertLocalToWorld(const Vector3& localPosition) const;

            inline ActorContainer* getParent() const { return parent; }
            void removeFromParent();

            virtual void addChild(Actor* actor) override;

            void addComponent(Component* component);

            template<typename T> void addComponent(const std::unique_ptr<T>& component)
            {
                addComponent(component.get());
            }

            template<typename T> void addComponent(std::unique_ptr<T>&& component)
            {
                addComponent(component.get());
                ownedComponents.push_back(std::move(component));
            }

            bool removeComponent(Component* component);

            template<typename T> void removeComponent(const std::unique_ptr<T>& component)
            {
                removeComponent(component.get());
            }

            void removeAllComponents();
            inline const std::vector<Component*>& getComponents() const { return components; }
            std::vector<Component*> getComponents(uint32_t cls) const;

            Box3 getBoundingBox() const;

        protected:
            virtual void setLayer(Layer* newLayer) override;

            void updateLocalTransform();
            void updateTransform(const Matrix4& newParentTransform);

            virtual void calculateLocalTransform() const;
            virtual void calculateTransform() const;

            virtual void calculateInverseTransform() const;

            Matrix4 parentTransform;
            mutable Matrix4 transform;
            mutable Matrix4 inverseTransform;
            mutable Matrix4 localTransform;

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

            Vector3 position;
            Quaternion rotation = Quaternion::identity();
            Vector3 scale = Vector3(1.0F, 1.0F, 1.0F);
            float opacity = 1.0F;
            int32_t order = 0;
            int32_t worldOrder = 0;

            ActorContainer* parent = nullptr;

            std::vector<Component*> components;
            std::vector<std::unique_ptr<Component>> ownedComponents;

            UpdateCallback animationUpdateCallback;
        };
    } // namespace scene
} // namespace ouzel
