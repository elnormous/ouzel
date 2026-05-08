#ifndef FLUXION_EDITOR_SCENETREEPANEL_HPP
#define FLUXION_EDITOR_SCENETREEPANEL_HPP

#include "../Panel.hpp"

namespace fluxion::scene
{
    class Actor;
    class ActorContainer;
    class Layer;
    class Scene;
}

namespace fluxion::editor
{
    class SelectionContext;

    // Hierarchical view of the editor's working scene: layers as
    // top-level collapsing headers, actors below each layer as a
    // recursive tree. Clicking an actor row updates the shared
    // SelectionContext so the inspector and other panels follow.
    //
    // The panel does not own the scene or the selection context — both
    // outlive every panel because they're EditorApp members. The panel
    // does not mutate the scene; actor add/remove will arrive in a
    // follow-up phase via a context menu.
    class SceneTreePanel final : public Panel
    {
    public:
        // `excludeLayer`/`excludeActor`, if non-null, are filtered
        // out of the hierarchy view — used by the editor to hide its
        // own internal helpers (back-buffer-clear camera, viewport
        // camera, future gizmos) so the tree shows only user-authored
        // content. `setExcludeActor` lets the caller swap the actor
        // pointer after construction (the viewport camera actor only
        // exists once the panel that owns it is built).
        SceneTreePanel(scene::Scene* scene,
                       SelectionContext* selection,
                       const scene::Layer* excludeLayer = nullptr,
                       const scene::Actor* excludeActor = nullptr);

        void setExcludeActor(const scene::Actor* a) noexcept
        {
            excludeActor_ = a;
        }

        void draw() override;

    private:
        // Recursively renders an Actor and its children. Implemented as
        // a free helper inside the .cpp; the public interface keeps the
        // header light.
        void drawActor(scene::Actor& actor);
        void drawContainerChildren(scene::ActorContainer& container);

        scene::Scene* scene_;                  // not owned
        SelectionContext* selection_;          // not owned
        const scene::Layer* excludeLayer_;     // not owned
        const scene::Actor* excludeActor_;     // not owned
    };
}

#endif // FLUXION_EDITOR_SCENETREEPANEL_HPP
