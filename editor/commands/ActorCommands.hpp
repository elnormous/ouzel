#ifndef FLUXION_EDITOR_COMMANDS_ACTORCOMMANDS_HPP
#define FLUXION_EDITOR_COMMANDS_ACTORCOMMANDS_HPP

#include <memory>
#include <string>
#include "Command.hpp"
#include "../../engine/utils/Uuid.hpp"

namespace fluxion::scene
{
    class Actor;
    class Layer;
}

namespace fluxion::editor
{
    class SelectionContext;
}

namespace fluxion::editor::commands
{
    // Adds a freshly-built Actor under either a Layer (top-level) or
    // an existing Actor (nested). On undo the Actor is detached from
    // its parent and held inside the command, ready for redo.
    //
    // Parent layer is referenced as a raw pointer (Layer instances
    // are EditorApp members — stable for the editor's lifetime).
    // Parent actor is referenced by Uuid + the same layer pointer so
    // we can re-resolve it after a delete-then-undo round trip
    // changes the actor's heap address.
    class AddActorCommand final : public Command
    {
    public:
        // Adds at the layer's top level.
        AddActorCommand(scene::Layer* parentLayer,
                        std::unique_ptr<scene::Actor> newActor,
                        SelectionContext* selection);

        // Adds nested under an existing actor (looked up by uuid).
        AddActorCommand(scene::Layer* parentLayer,
                        const Uuid& parentActorId,
                        std::unique_ptr<scene::Actor> newActor,
                        SelectionContext* selection);

        void redo() override;
        void undo() override;
        [[nodiscard]] std::string label() const override;

    private:
        scene::Layer* parentLayer_;     // not owned, stable
        Uuid parentActorId_;             // null when parent is the layer
        Uuid newActorId_;                // captured for re-resolve
        std::unique_ptr<scene::Actor> storedActor_;  // alternates owned/null
        SelectionContext* selection_;   // not owned; nullptr is OK
    };

    // Removes an actor from its parent. The detached Actor lives
    // inside the command between redo and undo. The actor's existing
    // identity (uuid, components, transform) is preserved exactly,
    // so undo restores the actor in the same state it was deleted
    // in.
    class DeleteActorCommand final : public Command
    {
    public:
        DeleteActorCommand(scene::Layer* parentLayer,
                           const Uuid& actorId,
                           SelectionContext* selection);

        void redo() override;
        void undo() override;
        [[nodiscard]] std::string label() const override;

    private:
        scene::Layer* parentLayer_;
        Uuid actorId_;
        // Captured the first time redo() runs: the parent actor's
        // uuid (or null if parent is the layer). Subsequent redo /
        // undo cycles use this without re-scanning.
        Uuid parentActorId_;
        bool parentResolved_ = false;
        std::unique_ptr<scene::Actor> storedActor_;
        SelectionContext* selection_;
    };
}

#endif // FLUXION_EDITOR_COMMANDS_ACTORCOMMANDS_HPP
