// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_COMPONENT_HPP
#define OUZEL_SCENE_COMPONENT_HPP

#include <memory>
#include <vector>
#include "../math/Box.hpp"
#include "../math/Matrix.hpp"
#include "../math/Color.hpp"
#include "../math/Rect.hpp"
#include "../graphics/Texture.hpp"

namespace ouzel::scene
{
    class Actor;
    class Layer;

    class Component
    {
        friend Actor;
    public:
        Component() = default;
        virtual ~Component();

        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;

        Component(Component&&) = delete;
        Component& operator=(Component&&) = delete;

        virtual void draw(const math::Matrix<float, 4>& transformMatrix,
                          float opacity,
                          const math::Matrix<float, 4>& renderViewProjection,
                          bool wireframe);

        virtual const math::Box<float, 3>& getBoundingBox() const noexcept { return boundingBox; }
        virtual void setBoundingBox(const math::Box<float, 3>& newBoundingBox) { boundingBox = newBoundingBox; }

        virtual bool pointOn(const math::Vector<float, 2>& position) const;
        virtual bool shapeOverlaps(const std::vector<math::Vector<float, 2>>& edges) const;

        [[nodiscard]] auto isHidden() const noexcept { return hidden; }
        void setHidden(bool newHidden) { hidden = newHidden; }

        [[nodiscard]] auto getActor() const noexcept { return actor; }
        void removeFromActor();

    protected:
        virtual void setActor(Actor* newActor);
        virtual void setLayer(Layer* newLayer);
        virtual void updateTransform();

        math::Box<float, 3> boundingBox;
        bool hidden = false;

        Layer* layer = nullptr;
        Actor* actor = nullptr;
    };
}

#endif // OUZEL_SCENE_COMPONENT_HPP
