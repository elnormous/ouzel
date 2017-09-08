// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include "utils/Noncopyable.hpp"
#include "math/Box3.hpp"
#include "math/Matrix4.hpp"
#include "math/Color.hpp"
#include "math/Rectangle.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        class Camera;
        class Actor;

        class Component: public Noncopyable
        {
            friend Actor;
        public:
            enum Type
            {
                NONE = 0,
                ANIMATOR = 1,
                MESH_RENDERER = 2,
                PARTICLE_SYSTEM = 3,
                SHAPE_RENDERER = 4,
                LISTENER = 5,
                SOUND = 6,
                SPRITE = 7,
                TEXT_RENDERER = 8
            };

            Component(uint32_t aType);
            virtual ~Component();

            uint32_t getType() const { return type; }

            virtual void draw(const Matrix4& transformMatrix,
                              float opacity,
                              const Matrix4& renderViewProjection,
                              const std::shared_ptr<graphics::Texture>& renderTarget,
                              const Rectangle& renderViewport,
                              bool depthWrite,
                              bool depthTest,
                              bool wireframe,
                              bool scissorTest,
                              const Rectangle& scissorRectangle);

            virtual void setBoundingBox(const Box3& newBoundingBox) { boundingBox = newBoundingBox; }
            virtual const Box3& getBoundingBox() const { return boundingBox; }

            virtual bool pointOn(const Vector2& position) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

            bool isHidden() const { return hidden; }
            void setHidden(bool newHidden) { hidden = newHidden; }

            Actor* getActor() const { return actor; }
            void removeFromActor();

        protected:
            uint32_t type;

            Box3 boundingBox;
            bool hidden = false;

            Actor* actor = nullptr;
        };
    } // namespace scene
} // namespace ouzel
