// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>
#include "scene/ActorContainer.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace scene
    {
        class Scene;
        class Camera;
        class Light;

        class Layer: public ActorContainer
        {
            friend Scene;
            friend Camera;
            friend Light;
        public:
            Layer();
            virtual ~Layer();

            virtual void draw();

            void addChild(Actor* actor) override;

            inline const std::vector<Camera*>& getCameras() const { return cameras; }

            std::pair<Actor*, Vector3> pickActor(const Vector2& position, bool renderTargets = false) const;
            std::vector<std::pair<Actor*, Vector3>> pickActors(const Vector2& position, bool renderTargets = false) const;
            std::vector<Actor*> pickActors(const std::vector<Vector2>& edges, bool renderTargets = false) const;

            inline int32_t getOrder() const { return order; }
            void setOrder(int32_t newOrder);

            inline Scene* getScene() const { return scene; }
            void removeFromScene();

        protected:
            void addCamera(Camera* camera);
            void removeCamera(Camera* camera);

            void addLight(Light* light);
            void removeLight(Light* light);

            virtual void recalculateProjection();
            void enter() override;

            Scene* scene = nullptr;

            std::vector<Camera*> cameras;
            std::vector<Light*> lights;

            int32_t order = 0;
        };
    } // namespace scene
} // namespace ouzel
