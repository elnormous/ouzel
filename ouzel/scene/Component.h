// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include "utils/Noncopyable.h"
#include "math/Box3.h"
#include "math/Matrix4.h"
#include "math/Color.h"

namespace ouzel
{
    namespace scene
    {
        class Camera;
        class Node;

        class Component: public Noncopyable
        {
            friend Node;
        public:
            virtual void draw(const Matrix4& transformMatrix,
                              const Color& drawColor,
                              Camera* camera);

            virtual void drawWireframe(const Matrix4& transformMatrix,
                                       const Color& drawColor,
                                       Camera* camera);

            virtual void setBoundingBox(const Box3& newBoundingBox) { boundingBox = newBoundingBox; }
            virtual const Box3& getBoundingBox() const { return boundingBox; }

            virtual bool pointOn(const Vector2& position) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

            bool isHidden() const { return hidden; }
            void setHidden(bool newHidden) { hidden = newHidden; }

            Node* getNode() const { return node; }

        protected:
            Box3 boundingBox;
            bool hidden = false;

            Node* node = nullptr;
        };
    } // namespace scene
} // namespace ouzel
