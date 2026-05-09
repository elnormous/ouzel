#ifndef FLUXION_EDITOR_INSPECTORPANEL_HPP
#define FLUXION_EDITOR_INSPECTORPANEL_HPP

#include <string>
#include "../Panel.hpp"

namespace fluxion::assets { class Cache; }
namespace fluxion::gui::imgui { class Context; }
namespace fluxion::scene
{
    class Actor;
    class Layer;
}

namespace fluxion::editor
{
    class AssetDatabase;
    class CommandStack;
    class SelectionContext;

    // Live inspector for the currently-selected Actor.
    //
    // Differs from the older ReflectionPanel in two ways:
    //   1. It edits a real Actor in the scene (mutations stick), not a
    //      throwaway temporary instance built from the TypeRegistry.
    //   2. It walks the selected Actor's components and calls inspect()
    //      on each, so anything reflection-aware shows up automatically
    //      (no per-component panel wiring required).
    //
    // Actor-level properties (id, name, transform, opacity, visibility
    // flags) are not Components and don't go through the reflection
    // Inspector — they're rendered with direct ImGui calls because the
    // editor needs read-modify-write semantics through the Actor's
    // public setters.
    class InspectorPanel final : public Panel
    {
    public:
        // userLayer + commandStack are optional. When both are
        // provided, name edits and component add / remove route
        // through the undo/redo system. Without them the panel
        // mutates the actor directly (legacy / test path).
        // assetDatabase is also optional — when present, the
        // inspector renders AssetReference fields with resolved
        // names + a drag-drop receptor and routes drops as
        // SetAssetCommand. When absent, asset fields fall back to
        // the engine-side raw uuid display.
        explicit InspectorPanel(SelectionContext* selection,
                                scene::Layer* userLayer = nullptr,
                                CommandStack* commandStack = nullptr,
                                const AssetDatabase* assetDatabase = nullptr,
                                assets::Cache* cache = nullptr,
                                gui::imgui::Context* imgui = nullptr);

        void draw() override;

    private:
        void drawActorHeader(scene::Actor& actor);
        void drawTransform(scene::Actor& actor);
        void drawDisplayFlags(scene::Actor& actor);
        void drawComponents(scene::Actor& actor);
        // Asset-selection branch: renders the .meta data for the
        // currently-selected AssetEntry plus a preview tile for
        // image assets. Used when the AssetBrowser sets the asset
        // selection channel on SelectionContext.
        void drawAssetInspector();

        SelectionContext* selection_;       // not owned
        scene::Layer* userLayer_;           // not owned; nullptr OK
        CommandStack* commandStack_;        // not owned; nullptr OK
        const AssetDatabase* assetDatabase_;// not owned; nullptr OK
        assets::Cache* cache_;              // not owned; nullptr OK
        gui::imgui::Context* imgui_;        // not owned; nullptr OK

        // Captured-on-activate value for the name InputText. Used
        // to push a SetNameCommand on commit (IsItemDeactivatedAfterEdit)
        // so a single rename session is one undo step regardless of
        // how many keystrokes happen during it.
        std::string nameEditOldValue_;
        bool nameEditCapturing_ = false;
    };
}

#endif // FLUXION_EDITOR_INSPECTORPANEL_HPP
