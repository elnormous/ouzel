#ifndef FLUXION_EDITOR_SELECTIONCONTEXT_HPP
#define FLUXION_EDITOR_SELECTIONCONTEXT_HPP

#include <cstddef>
#include <functional>
#include <vector>

namespace fluxion::scene
{
    class Actor;
    class Scene;
}

namespace fluxion::editor
{
    // Single-actor selection state shared across editor panels.
    //
    // Lifetime:
    //   * The owner (EditorApp) holds the SelectionContext by value and
    //     hands references to every panel that needs it.
    //   * The pointer returned by getSelected() is non-owning and refers
    //     to an Actor in the editor's working scene. Callers MUST NOT
    //     dereference it after the actor leaves the scene; the editor
    //     code that mutates the scene (add/remove actor in later phases)
    //     is responsible for clearing the selection beforehand. The
    //     validateAgainst() helper makes that responsibility easy.
    //
    // Threading:
    //   * Read/write only from the render thread, where ImGui draws and
    //     all panels run. Observer callbacks fire synchronously from
    //     inside setSelected(). No internal locking.
    class SelectionContext
    {
    public:
        using Listener = std::function<void(scene::Actor* selected)>;
        using ListenerId = std::size_t;

        SelectionContext() = default;
        ~SelectionContext() = default;

        SelectionContext(const SelectionContext&) = delete;
        SelectionContext& operator=(const SelectionContext&) = delete;
        SelectionContext(SelectionContext&&) = delete;
        SelectionContext& operator=(SelectionContext&&) = delete;

        [[nodiscard]] scene::Actor* getSelected() const noexcept { return selected_; }
        [[nodiscard]] bool hasSelection() const noexcept { return selected_ != nullptr; }

        // Updates the selection. Fires listeners only when the value
        // actually changes — duplicate selects on the same actor are
        // ignored, which keeps the inspector from rebuilding state on
        // every tree-row redraw.
        void setSelected(scene::Actor* actor);

        void clear() { setSelected(nullptr); }

        // Walks `scene` and clears the selection if the currently
        // selected pointer is no longer in any layer. Cheap, intended
        // to be called once per frame from the SceneTreePanel before
        // it renders. Robust against actor removal that happens between
        // frames without going through the editor's own remove path.
        void validateAgainst(const scene::Scene& scene);

        // Subscribe to selection changes. The returned id can be passed
        // to removeListener() to detach. Listener IDs never wrap and
        // never alias previously-removed slots.
        ListenerId addListener(Listener listener);
        void removeListener(ListenerId id);

    private:
        void notify();

        scene::Actor* selected_ = nullptr;

        struct ListenerSlot
        {
            ListenerId id;
            Listener fn;
        };

        std::vector<ListenerSlot> listeners_;
        ListenerId nextListenerId_ = 1;
    };
}

#endif // FLUXION_EDITOR_SELECTIONCONTEXT_HPP
