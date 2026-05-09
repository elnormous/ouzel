#ifndef FLUXION_SCENE_COMPONENT_HPP
#define FLUXION_SCENE_COMPONENT_HPP

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "../graphics/Texture.hpp"
#include "../math/Box.hpp"
#include "../math/Matrix.hpp"
#include "../math/Color.hpp"
#include "../math/Rect.hpp"

namespace fluxion::reflection
{
    class Inspector;
}

namespace fluxion::scene
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

        // Reflection: stable, user-facing type identifier (e.g. "scene.SpriteRenderer").
        // Subclasses normally provide this via the FLUXION_COMPONENT_DECL macro
        // along with a factory entry in the global TypeRegistry. The default
        // implementation returns an empty view, which keeps non-reflective
        // components compilable without any source change.
        [[nodiscard]] virtual std::string_view typeName() const noexcept { return {}; }

        // Reflection: report this component's editable properties to the
        // given inspector. Default is a no-op so existing components keep
        // working; reflection-aware ones override this to call inspector's
        // typed-field methods in the order the editor should show them.
        virtual void inspect(reflection::Inspector& inspector) { (void)inspector; }

        [[nodiscard]] const auto& getBoundingBox() const noexcept { return boundingBox; }
        void setBoundingBox(const math::Box<float, 3>& newBoundingBox) noexcept
        {
            boundingBox = newBoundingBox;
        }

        bool pointOn(const math::Vector<float, 2>& position) const noexcept;
        bool shapeOverlaps(const std::vector<math::Vector<float, 2>>& edges) const noexcept;

        [[nodiscard]] auto isHidden() const noexcept { return hidden; }
        void setHidden(bool newHidden) { hidden = newHidden; }

        // Sorting controls (used by 2D renderers — Sprite, Shape,
        // Text). Components that don't render geometry (Animator,
        // Camera, Light) inherit the fields too but ignore them.
        // Resolved against the project-wide SortingLayers registry
        // when the renderer queue is sorted.
        [[nodiscard]] const std::string& getSortingLayer() const noexcept { return sortingLayer; }
        void setSortingLayer(std::string newLayer) noexcept { sortingLayer = std::move(newLayer); }
        [[nodiscard]] int getOrderInLayer() const noexcept { return orderInLayer; }
        void setOrderInLayer(int newOrder) noexcept { orderInLayer = newOrder; }

        [[nodiscard]] auto getActor() const noexcept { return actor; }
        void removeFromActor();

    protected:
        virtual void setActor(Actor* newActor);
        virtual void setLayer(Layer* newLayer);
        virtual void updateTransform();

        // Subclass helper: surfaces the sortingLayer + orderInLayer
        // fields through an Inspector. Keeps the field labels and
        // ordering uniform across renderer subclasses without
        // forcing every component to remember the magic strings.
        // Called explicitly from a subclass's inspect() override
        // when it wants the fields exposed.
        void inspectSorting(reflection::Inspector& inspector);

        math::Box<float, 3> boundingBox;
        bool hidden = false;
        std::string sortingLayer = "Default";
        int orderInLayer = 0;

        Layer* layer = nullptr;
        Actor* actor = nullptr;
    };
}

#endif // FLUXION_SCENE_COMPONENT_HPP
